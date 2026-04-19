#ifndef NOISE_SEEDER_H
#define NOISE_SEEDER_H

#include "modules/noise/noise.h"

class NoiseSeeder : public Resource {
	GDCLASS(NoiseSeeder, Resource);
	OBJ_SAVE_TYPE(NoiseSeeder);

public:
	NoiseSeeder() {}
	virtual ~NoiseSeeder() {}

	void set_noise(Ref<Noise> n);
	_FORCE_INLINE_ Ref<Noise> get_noise() const;

	void set_seed(int s);
	_FORCE_INLINE_ int get_seed() const;

protected:
	static void _bind_methods();

private:
	Ref<Noise> noise;
	int seed{ 0 };
};

#endif