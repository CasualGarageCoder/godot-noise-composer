#ifndef VISUAL_NOISE_EDITOR_H
#define VISUAL_NOISE_EDITOR_H

#include "../visual_noise.h"

#include "editor/editor_node.h"
#include "scene/gui/button.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/popup.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/tree.h"

class VisualNoiseEditor : public GraphEdit {
	GDCLASS(VisualNoiseEditor, GraphEdit)

public:
	VisualNoiseEditor();
	virtual ~VisualNoiseEditor();

	void set_noise(Ref<VisualNoise> n);
	Ref<VisualNoise> get_noise() const;

protected:
	static void _bind_methods();

private:
	static VisualNoiseEditor *instance;

	Ref<VisualNoise> noise;
};

class VisualNoiseEditorPlugin : public EditorPlugin {
	GDCLASS(VisualNoiseEditorPlugin, EditorPlugin);

public:
	virtual String get_plugin_name() const override { return "VisualNoise"; }
	virtual void edit(Object *object) override;
	virtual bool handles(Object *object) const override;
	virtual void make_visible(bool visible) override;

private:
	VisualNoiseEditor *editor;
	EditorNode *node;
	Button *button;
};

#endif