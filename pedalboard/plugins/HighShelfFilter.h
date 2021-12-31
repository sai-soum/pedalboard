/*
 * pedalboard
 * Copyright 2021 Spotify AB
 *
 * Licensed under the GNU Public License, Version 3.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    https://www.gnu.org/licenses/gpl-3.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include "../JucePlugin.h"

namespace Pedalboard {
template <typename SampleType>
class HighShelfFilter : public JucePlugin<juce::dsp::IIR::Filter<SampleType>> {
public:
  void setCutoffFrequencyHz(float f) noexcept { cutoffFrequencyHz = f; }
  float getCutoffFrequencyHz() const noexcept { return cutoffFrequencyHz; }
  void setQ(float f) noexcept { Q = f; }
  float getQ() const noexcept { return Q; }
  void setGainFactor(float f) noexcept { gainFactor = f; }
  float getGainFactor() const noexcept { return gainFactor; }


  virtual void prepare(const juce::dsp::ProcessSpec &spec) override {
    JucePlugin<juce::dsp::IIR::Filter<SampleType>>::prepare(spec);
    this->getDSP().coefficients =
        juce::dsp::IIR::Coefficients<SampleType>::makeHighShelf(
            spec.sampleRate, cutoffFrequencyHz, Q, gainFactor);
  }

private:
  float cutoffFrequencyHz;
  float Q;
  float gainFactor;
};

inline void init_highshelf(py::module &m) {
  py::class_<HighShelfFilter<float>, Plugin>(
      m, "HighShelfFilter",
      "Apply a high-pass shelf filter with variable Q and gain. "
      "The gain is a scale factor that the high frequencies are multiplied by, so values"
      "greater than 1.0 will boost the high frequencies, values less than 1.0 will"
      "attenuate them.")
      .def(py::init([](float cutoff_frequency_hz, float Q_val, float gainFactorBoost) {
             auto plugin = new HighShelfFilter<float>();
             plugin->setCutoffFrequencyHz(cutoff_frequency_hz);
             plugin->setQ(Q_val);
             plugin->setGainFactor(gainFactorBoost);
             return plugin;
           }),
           py::arg("cutoff_frequency_hz") = 50,
           py::arg("Q") = 0.707,
           py::arg("gain_factor") = 1.0)
      .def("__repr__",
           [](const HighShelfFilter<float> &plugin) {
             std::ostringstream ss;
             ss << "<pedalboard.HighShelf";
             ss << " cutoff_frequency_hz=" << plugin.getCutoffFrequencyHz();
             ss << " Q=" << plugin.getQ();
             ss << " gain_factor=" << plugin.getGainFactor();
             ss << " at " << &plugin;
             ss << ">";
             return ss.str();
           })
      .def_property("cutoff_frequency_hz",
                    &HighShelfFilter<float>::getCutoffFrequencyHz,
                    &HighShelfFilter<float>::setCutoffFrequencyHz)
      .def_property("Q", 
                    &HighShelfFilter<float>::getQ,
                    &HighShelfFilter<float>::setQ)
      .def_property("gain_factor", 
                    &HighShelfFilter<float>::getGainFactor,
                    &HighShelfFilter<float>::setGainFactor);
}
}; // namespace Pedalboard