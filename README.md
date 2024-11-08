# Fluid Multitouch Simulation

A creative multitouch fluid simulation built with OpenFrameworks, using MSAFluid for fluid dynamics and supporting native Linux multitouch input.

## Features

- Real-time fluid simulation responding to touch input
- True multitouch support on Linux/Wayland
- Multiple visualization modes:
  - Color rendering
  - Motion vectors
  - Speed visualization
  - Vector field display
- Customizable parameters via GUI:
  - Fluid viscosity
  - Fade speed
  - Color diffusion
  - Input size
  - Force multiplier
- Layer combination with adjustable opacity
- Optional GNOME gesture disabling for better touch interaction
- Time-based effects for each touch point

## Dependencies

- OpenFrameworks (latest version)
- ofxMSACore
- ofxMSAFluid 
- ofxGui (included with OF)
- ofxMultitouch
- libinput (for Linux touch support)
- gsettings (for GNOME gesture control)

## Installation

1. Clone this repository into OpenFrameworks' `apps/myApps` directory:
```bash
cd openFrameworks/apps/myApps
git clone [repository-url] fmSim
```

2. Install system dependencies (Fedora):
```bash
sudo dnf install libinput-devel gsettings-desktop-schemas
```

3. Add your user to the input group for touch access:
```bash
sudo usermod -aG input your_username
```

4. Build and run:
```bash
cd fmSim
make
make RunRelease
```

## Controls

- Mouse/Touch: Draw fluid
- 'g': Toggle GUI
- 'v': Toggle vector display
- 'm': Toggle motion display
- 'c': Toggle color display
- 's': Toggle speed display
- '[' / ']': Decrease/Increase input size

## GUI Parameters

- Velocity Multiplier: Scales the effect of movement speed
- Viscosity: Controls fluid thickness
- Fade Speed: How quickly the fluid dissipates
- Delta T: Time step for simulation
- Color Diffusion: How colors blend together
- Force Multiplier: Scales the force applied to the fluid
- Input Size: Size of touch influence
- Layer Opacity: Transparency of combined visualization modes

## Credits

Built with:
- [OpenFrameworks](https://openframeworks.cc/)
- [MSAFluid](https://github.com/memo/ofxMSAFluid)
- [ofxMultitouch](https://github.com/fx-lange/ofxMultitouch)

## License

MIT License
