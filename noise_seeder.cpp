#include "noise_seeder.h"
#include "noise_base.h"
#include <cstdint>
#include <map>
#include <queue>
#include <random>
#include <set>

void NoiseSeeder::set_noise(Ref<Noise> n) {
	noise = n;
	if (noise.is_valid()) {
		set_seed(seed);
	}
}
Ref<Noise> NoiseSeeder::get_noise() const { return noise; }

void NoiseSeeder::set_seed(int s) {
	seed = s;
	std::mt19937 rand_e;
	rand_e.seed(seed);
	std::uniform_int_distribution<int> rand_d;

	if (!noise.is_valid()) {
		return;
	}

	std::map<uint64_t, Ref<Noise>> to_seed;
	std::set<Ref<Noise>> checked;
	std::queue<Ref<Noise>> to_check;

	to_check.push(noise);

	while (!to_check.empty()) {
		Ref<Noise> t = to_check.front();
		if (!t.is_valid()) {
			continue;
		}
		to_check.pop();

		if (t->is_class("NoiseNode")) {
			// It's pretty secure at this point ...
			NoiseNode *node = reinterpret_cast<NoiseNode *>(t.ptr());
			for (auto n : *node) {
				auto it = checked.find(n);
				if (it == checked.end()) {
					to_check.push(n);
					checked.insert(n);
				}
			}
		} else if (t->has_method("set_seed")) {
			to_seed[t->get_instance_id()] = t;
		}
	}

	for (auto e : to_seed) {
		int local_seed = rand_d(rand_e);
		e.second->call_deferred("set_seed", local_seed);
	}
}

int NoiseSeeder::get_seed() const { return seed; }

void NoiseSeeder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_noise", "n"), &NoiseSeeder::set_noise);
	ClassDB::bind_method(D_METHOD("get_noise"), &NoiseSeeder::get_noise);

	ClassDB::bind_method(D_METHOD("set_seed", "s"), &NoiseSeeder::set_seed);
	ClassDB::bind_method(D_METHOD("get_seed"), &NoiseSeeder::get_seed);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "noise",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_noise", "get_noise");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
}