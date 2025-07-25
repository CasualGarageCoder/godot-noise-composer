#include "visual_noise_editor_plugin.h"

VisualNoiseEditor *VisualNoiseEditor::instance = nullptr;

VisualNoiseEditor::VisualNoiseEditor() {
	instance = this;
	// FIXME Free previous instance ?
}

VisualNoiseEditor::~VisualNoiseEditor() {
	// FIXME
}