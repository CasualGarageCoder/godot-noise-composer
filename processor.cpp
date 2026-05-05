#include "processor.h"

#include "core/math/math_defs.h"
#include "core/object/callable_mp.h"
#include "core/object/class_db.h"

#include <cmath>
#include <random>

real_t DerivativeNoise::get_noise_1d(real_t p_x) const {
	if (source.is_valid()) {
		real_t prev = source->get_noise_1d(p_x - distance);
		real_t next = source->get_noise_1d(p_x + distance);
		return (next - prev) / (2. * distance);
	}
	return 0.;
}

real_t DerivativeNoise::get_noise_2dv(Vector2 p_v) const {
	return get_noise_2d(p_v.x, p_v.y);
}

real_t DerivativeNoise::get_noise_2d(real_t p_x, real_t p_y) const {
	if (source.is_valid()) {
		real_t px = source->get_noise_2d(p_x - distance, p_y);
		real_t nx = source->get_noise_2d(p_x + distance, p_y);
		real_t py = source->get_noise_2d(p_x, p_y - distance);
		real_t ny = source->get_noise_2d(p_x, p_y + distance);
		return (nx - px + ny - py) / (4. * distance);
	}
	return 0.;
}

real_t DerivativeNoise::get_noise_3dv(Vector3 p_v) const {
	return get_noise_3d(p_v.x, p_v.y, p_v.z);
}

real_t DerivativeNoise::get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const {
	if (source.is_valid()) {
		real_t px = source->get_noise_3d(p_x - distance, p_y, p_z);
		real_t nx = source->get_noise_3d(p_x + distance, p_y, p_z);
		real_t py = source->get_noise_3d(p_x, p_y - distance, p_z);
		real_t ny = source->get_noise_3d(p_x, p_y + distance, p_z);
		real_t pz = source->get_noise_3d(p_x, p_y, p_z - distance);
		real_t nz = source->get_noise_3d(p_x, p_y, p_z + distance);
		return (nx - px + ny - py + nz - pz) / (6. * distance);
	}
	return 0.;
}

void DerivativeNoise::set_distance(real_t v) {
	distance = v;
	emit_changed();
}

void DerivativeNoise::set_source(Ref<Noise> s) {
	if (source.is_valid()) {
		source->disconnect_changed(callable_mp(this, &DerivativeNoise::_changed));
	}
	source = s;
	if (source.is_valid()) {
		source->connect_changed(callable_mp(this, &DerivativeNoise::_changed), CONNECT_DEFERRED);
	}
	emit_changed();
}

void DerivativeNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_distance", "v"), &DerivativeNoise::set_distance);
	ClassDB::bind_method(D_METHOD("get_distance"), &DerivativeNoise::get_distance);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_source", "get_source");

	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "distance",
					PROPERTY_HINT_RANGE, "0.001,1,0.001"),
			"set_distance", "get_distance");
}

void SmoothNoise::set_source(Ref<Noise> s) {
	if (source.is_valid()) {
		source->disconnect_changed(callable_mp(this, &SmoothNoise::_changed));
	}
	source = s;
	if (source.is_valid()) {
		source->connect_changed(callable_mp(this, &SmoothNoise::_changed), CONNECT_DEFERRED);
	}
	emit_changed();
}

real_t SmoothNoise::get_noise_1d(real_t p_x) const {
	std::shared_lock<std::shared_mutex> lock(*(const_cast<std::shared_mutex *>(&bake_mutex)));
	real_t sum = source.is_valid() ? source->get_noise_1d(p_x) : 0.;
#pragma omp parallel for reduce(+ : sum)
	for (int i = 0; i < sample_count; ++i) {
		Vector2 v = kernel_1d[i];
		sum += source.is_valid() ? source->get_noise_1d(p_x + v.x) * v.y : 0.;
	}
	return sum / (weight_1d + 1.);
}

real_t SmoothNoise::get_noise_2dv(Vector2 p_v) const {
	return get_noise_2d(p_v.x, p_v.y);
}

real_t SmoothNoise::get_noise_2d(real_t p_x, real_t p_y) const {
	std::shared_lock<std::shared_mutex> lock(*(const_cast<std::shared_mutex *>(&bake_mutex)));
	real_t sum = source.is_valid() ? source->get_noise_2d(p_x, p_y) : 0.;
#pragma omp parallel for reduce(+ : sum)
	for (int i = 0; i < sample_count; ++i) {
		Vector3 v = kernel_2d[i];
		sum += source.is_valid() ? source->get_noise_2d(p_x + v.x, p_y + v.y) * v.z : 0.;
	}
	real_t result = sum / (weight_2d + 1.);
	return result;
}

real_t SmoothNoise::get_noise_3dv(Vector3 p_v) const {
	return get_noise_3d(p_v.x, p_v.y, p_v.z);
}

real_t SmoothNoise::get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const {
	std::shared_lock<std::shared_mutex> lock(*(const_cast<std::shared_mutex *>(&bake_mutex)));
	real_t sum = source.is_valid() ? source->get_noise_3d(p_x, p_y, p_z) : 0.;
#pragma omp parallel for reduce(+ : sum)
	for (int i = 0; i < sample_count; ++i) {
		Vector4 v = kernel_3d[i];
		sum += source.is_valid() ? source->get_noise_3d(p_x + v.x, p_y + v.y, p_z + v.z) * v.w : 0.;
	}
	return sum / (weight_3d + 1.);
}

void SmoothNoise::set_distance(real_t v) {
	distance = v;
	compute_kernels();
	emit_changed();
}

void SmoothNoise::set_sample_count(int v) {
	sample_count = std::max(1, v);
	compute_kernels();
	emit_changed();
}

void SmoothNoise::compute_kernels() {
	std::unique_lock<std::shared_mutex> lock(bake_mutex);

	if (kernel_1d != nullptr) {
		delete[] kernel_1d;
		delete[] kernel_2d;
		delete[] kernel_3d;
	}
	kernel_1d = new Vector2[sample_count];
	kernel_2d = new Vector3[sample_count];
	kernel_3d = new Vector4[sample_count];
	weight_1d = weight_2d = weight_3d = 0.;

	// initialize random
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rd_distance(0.001, MAX(0.001, distance));
	std::uniform_real_distribution<> rd_angle(0., 2. * Math::PI);
	std::uniform_real_distribution<> rd_azimuth(-Math::PI / 2., Math::PI / 2.);

	// Won't use openmp here 'cause it requires a generator and distribution per thread
	// and it relies on something else than #pragmas which is not portable.
	for (int i = 0; i < sample_count; ++i) {
		double d = rd_distance(gen);
		double ratio;
		ratio = (d / distance);
		ratio = Math::sqrt(1. - (ratio * ratio));
		kernel_1d[i] = Vector2((d * 2.) - 1., ratio);
		weight_1d += ratio;

		d = rd_distance(gen);
		double angle = rd_angle(gen);
		Vector2 dir(cos(angle), sin(angle));
		dir *= d;
		ratio = (d / distance);
		ratio = Math::sqrt(1. - (ratio * ratio));
		kernel_2d[i] = Vector3(dir.x, dir.y, ratio);
		weight_2d += ratio;

		d = rd_distance(gen);
		angle = rd_angle(gen);
		double azimuth = rd_azimuth(gen);
		Vector3 dir3 = Vector3(1., 0., 0.)
							   .rotated(Vector3(0., 0., 1.), azimuth)
							   .rotated(Vector3(0., 1., 0.), angle);
		dir3 *= d;
		ratio = (d / distance);
		ratio = Math::sqrt(1. - (ratio * ratio));
		kernel_3d[i] = Vector4(dir3.x, dir3.y, dir3.z, ratio);
		weight_3d += ratio;
	}
}

void SmoothNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_distance", "v"), &SmoothNoise::set_distance);
	ClassDB::bind_method(D_METHOD("get_distance"), &SmoothNoise::get_distance);

	ClassDB::bind_method(D_METHOD("set_sample_count", "v"), &SmoothNoise::set_sample_count);
	ClassDB::bind_method(D_METHOD("get_sample_count"), &SmoothNoise::get_sample_count);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_source", "get_source");

	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "distance",
					PROPERTY_HINT_RANGE, "0.001,8.,0.001"),
			"set_distance", "get_distance");

	ADD_PROPERTY(
			PropertyInfo(Variant::INT, "sample_count",
					PROPERTY_HINT_RANGE, "0,64,1"),
			"set_sample_count", "get_sample_count");
}