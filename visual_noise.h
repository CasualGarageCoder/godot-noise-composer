#ifndef VISUAL_NOISE_H
#define VISUAL_NOISE_H

#include "core/object/object.h"
#include "noise_composer.h"

// Visual Noise node only have ONE output (and possibly no inputs).
// One of these nodes will be designated as the output.
class VisualNoiseNode : public Resource {
	GDCLASS(VisualNoiseNode, Resource);

public:
	enum NodeType {
		NODE_NOISE,
		NODE_FLOAT,
		NODE_VECTOR2,
		NODE_VECTOR3,
		NODE_TRANSFORM2D,
		NODE_TRANSFORM3D,
		NODE_CURVE
	};

public:
	VisualNoiseNode() {}

	virtual ~VisualNoiseNode() {}

	virtual size_t get_input_count() const = 0;

	virtual NodeType get_input_type(size_t idx) const = 0;

	virtual void set_input(size_t idx, const Variant &value);
	virtual Variant get_input(size_t idx) const;

	// Output type is actually the node type itself.
	virtual NodeType get_output_type() const = 0;
	virtual Variant get_output() const = 0;

protected:
	static void _bind_methods();

private:
	Vector<Variant> inputs;
};

// Visual Noise

class VisualNoise : public Noise {
	GDCLASS(VisualNoise, Noise);
	OBJ_SAVE_TYPE(VisualNoise);

public:
protected:
	static void _bind_methods();

private:
	// The one and only output.
	Ref<VisualNoiseNode> output;
};

#endif