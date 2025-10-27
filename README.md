The designed plugin is a **VST Flanger effect**, featuring several configurable options to shape the sound. Its main architecture is divided into **four functional sections**:

---

### **Delay and Feedback**

The core of the flanger consists of **delaying the input signal**.

The main parameters are **Delay Time** and **Feedback**, which respectively determine the delay duration and the amount of delayed signal fed back into the buffer.

---

### **LFO Modulation**

As a flanger, the delayed signal is modulated using an **LFO (Low Frequency Oscillator)**.

The user can control:

* **Amount / Depth** – the intensity of the modulation.
* **Waveform** – the LFO waveform (Sine, Triangle, Saw, Square, etc.).
* **Phase Delta** – the modulation phase offset between stereo channels, to create spatial effects.

---

### **Optional Filter**

The modulated signal can be routed through a **filter**.

Configurable parameters:

* **Cutoff** – filter cutoff frequency.
* **Quality / Resonance** – filter resonance.
* **Filter Type** – LowPass, HighPass, or BandPass.

The filter can be **enabled or disabled** at the user’s discretion.

---

### **Mix & Gain**

* **Dry/Wet control** to balance the original and processed signals.
* **Output Gain control** to adjust the overall volume of the plugin.

Additionally, **Undo/Redo buttons** have been added to make parameter editing more user-friendly.

---

### **Stereo Signal Handling**

The plugin supports **stereo processing**, allowing for **phase delta modulation**.

This enables phase offset modulation between the two channels, enhancing the **sense of movement and spatial depth** typical of a flanger effect.
