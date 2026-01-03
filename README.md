<div align="center">

# Tuggle
### Mobile fidget toys built on CUGL

<p>
  <img width="280" height="1024" alt="logo" src="https://github.com/user-attachments/assets/2e696841-ba03-4846-89ea-ce1156032fd9" />
  <img width="280" height="300" alt="qr" src="https://github.com/user-attachments/assets/065e3c18-29ed-48d9-83fd-52cd92ad479c" />
</p>
</div>

## Credits

- Inspired by [Camdan Mead's](https://www.camdan.me/) fidget app, **[Fidgetable](https://apps.apple.com/us/app/fidgetable-haptic-fidget-toy/id6503308266)**
- Logo designed by [Linda Hu](https://linda-ly-h.vercel.app/)

## Motivation
This demo showcases the haptic features enabled by `tugl` and demonstrates how to create responsive iOS haptics using CUGL. The goal is to provide concrete, interactive examples of the different haptic styles and patterns supported by the library.

Each fidget toy explores a distinct aspect of haptic design, including using motion, velocity, and physics-based interactions to drive responsive feedback, as well as creating custom haptic waveforms and modulating them in real time. Together, these demos illustrate how haptics can be tightly coupled with user input and simulation state to enhance tactile feedback.

While the examples presented here do not exhaust the full capabilities of `tugl`, they are intended to serve as practical references and starting points for developers interested in integrating expressive, real-time haptics into their own projects.

## Usage
There are two ways to access the tuggle demo:
- Download from this [Testflight link](https://testflight.apple.com/join/EKjygjvG).
- Clone this repo and build with the included version of CUGL in this repo using the command `python cugl tuggle-demo` at the root that contains both folders.

## Tuggables
Tuggables are toys that can be found in the tuggle demo. There are 10 pages on the app, corresponding to each Tuggable available, demonstrating different functionalities enabled by `tugl`.

<table>
  <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/3dad5928-e61b-45ad-ab85-b394d4f2a79c" alt="App Screenshot" width="280">
    </td>
    <td>
      <h3>Tuggable 1: Tancho</h3>
      <p>
        Tancho is a singular button that slides. Tapping on it triggers a haptic effect, and sliding the button creates a rattling effect based on the sliding intensity. Its name is inspired by the iconic Tancho pattern seen in Japanese Nishikigoi.
      </p>
    </td>
  </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/ae873f75-242b-48f3-9660-e27e9549a7cf" alt="App Screenshot" width="280">
    <td>
      <h3>Tuggable 2: Sampler</h3>
      <p>
        Sampler is a collection of seven sliding buttons. Tapping on any of them triggers a haptic effect, and sliding the button creates a rattling effect based on the sliding intensity. Have a taste of what <code>tugl</code> has to offer!
      </p>
    </td>
  </tr>
    </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/47553106-9891-46a1-a609-42578727a5e0" alt="App Screenshot" width="280">
    <td>
      <h3>Tuggable 3: Vibrations</h3>
      <p>
        Vibrations is a collection of six toggleable buttons. Toggling them produces a continuous rumble effect, each varying in intensity and sharpness. They increase in intensity row-wise, with the left column being low sharpness and the right column being high sharpness.
      </p>
    </td>
  </tr>
    </tr>
    </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/c7c8b865-fcb6-4322-af0e-a872cbb24230" alt="App Screenshot" width="280">
    <td>
      <h3>Tuggable 4: Bubbles</h3>
      <p>
        Bubbles is a set of three buttons that invite exploration through touch. When held, each button slowly grows in size, building tension through progressive haptic feedback. At full expansion, the interaction resolves with a satisfying pop, simulating the tactile feel of a bubble bursting.
      </p>
    </td>
    </tr>
    </tr>
    </tr>
    </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/39d2008a-9cd3-44b0-b2e2-cf64a6e414f3" width="280">
    <td>
      <h3>Tuggable 5: Fellowship</h3>
      <p>
        Fellowship is a ring-shaped slider with a movable knob. As the knob travels around the ring’s perimeter, it emits distinct haptic ticks, because one does not simply slide through the ring without feeling every step.
      </p>
    </td>
    </tr>
    </tr>
    </tr>
    </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/75eb7c1b-4fdc-4201-a91b-78ea559e3841" alt="App Screenshot" width="280">
    <td>
      <h3>Tuggable 6: Katamari</h3>
      <p>
        Katamari simulates a ball rolling on a table, colliding with and bouncing off the walls that contain it. Haptics respond to surface friction and impacts, capturing the playful, physical feel of rolling something around a confined space.
      </p>
    </td>
    </tr>
    </tr>
    </tr>
    </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/7838faa7-2746-4979-9ae6-ff715c6b6524" alt="App Screenshot" width="280">
    <td>
      <h3>Tuggable 7: Fluid</h3>
      <p>
        Fluid is an exploratory simulation of particles in motion, interacting through collisions with each other and their container. The haptic output investigates the core sensations that could form the basis of a fluid-like haptic experience.
      </p>
    </td>
    </tr>
    </tr>
    </tr>
    </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/5bfc49fd-80f4-4eeb-84ea-3c643d2c5002" alt="App Screenshot" width="280">
    <td>
      <h3>Tuggable 8: Uppity</h3>
      <p>
        Uppity is a needle that always points upwards no matter how a user tilts their phone.
      </p>
    </td>
    </tr>
    </tr>
    </tr>
    </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/8a548044-073f-4410-b41f-a7d2a8a029fa" alt="App Screenshot" width="280">
    <td>
      <h3>Tuggable 9: Soundboard</h3>
      <p>
        Soundboard is a collection of nine buttons, each playing a sound clip. A haptic pattern from an AHAP file plays the waveform pattern representation of the sound simultaneously. These were created using <a href="https://developers.meta.com/horizon/resources/haptics-studio">Meta Haptics Studio</a>. Look here for a good example on how to play custom haptic patterns in <code>cugl</code>.
      </p>
    </td>
    </tr>
    </tr>
    </tr>
    </tr>
    <tr>
    <td width="300">
      <img src="https://github.com/user-attachments/assets/c1c22f25-aeb3-4914-9874-da912ae22619" width="280">
    <td>
      <h3>Tuggable 10: Verstappen</h3>
      <p>
        Verstappen simulates a car engine, allowing the user to control throttle input and shift gears up or down. Shifting up too early causes the engine to stall. This demo showcases how the <code>HapticPlayer</code> can be used to play continuous haptics and modulate them in real time.
      </p>
    </td>
    </tr>
</table>
