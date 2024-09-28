/**************************************************************************/
/* No Copyright, CC0                                                      */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "register_types.h"

#include "noise_composer.h"
#include "noise_seeder.h"

void initialize_noise_composer_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(ConstantNoise);
		GDREGISTER_CLASS(AddNoise);
		GDREGISTER_CLASS(MultiplyNoise);
		GDREGISTER_CLASS(MaxNoise);
		GDREGISTER_CLASS(MinNoise);
		GDREGISTER_CLASS(PowerNoise);
		GDREGISTER_CLASS(AbsoluteNoise);
		GDREGISTER_CLASS(InvertNoise);
		GDREGISTER_CLASS(ClampNoise);
		GDREGISTER_CLASS(CurveNoise);
		GDREGISTER_CLASS(AffineNoise);
		GDREGISTER_CLASS(MixNoise);
		GDREGISTER_CLASS(SelectNoise);
		GDREGISTER_CLASS(NoiseProxy);
		GDREGISTER_CLASS(LinearTransformNoise);
		GDREGISTER_CLASS(RescalerNoise);

		GDREGISTER_CLASS(NoiseSeeder)
	}
}

void uninitialize_noise_composer_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}
