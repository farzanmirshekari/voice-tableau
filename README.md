<h1>Voice Tableau</h1>

_**Voice Tableau**_ is a real-time sound spectrogram generator, leveraging _PortAudio_ to capture input audio to the device microhphone, _FFTW3_ to perform
a Fast Fourier Transform on the audio data and _OpenCV_ to visualize the results. Instead of the traditional rectangular visualization, _**Voice Tableau**_
creates an aesthetic circular representation of the input audio.

---

<h3>Origins</h3>

_**Voice Tableau**_ is a product of the mind of Mohammad Hekmat - my uncle - who was inspired by the works of Mexican-Canadian artist Rafael Lozano-Hemmer.

---

<h3>How to generate a <b><i>Voice Tableau</i></b></h3>

To build the project, make sure to have the following dependencies installed:

<ul>
  <li>PortAudio</li>
  <li>FFTW3</li>
  <li>OpenCV</li>
</ul>

The available Makefile is for Mac users (tested and verified on <b>OSX Ventura</b>).
<br>
The project can be built with:
~~~
Make
~~~
and run with:
~~~
./voice_tableau
~~~

---

<i>Developed by Farzan Mirshekari & Mohammad Hekmat</i>