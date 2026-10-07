# Contributing to Line Follower Bot

Thank you for your interest in contributing! This is an open hardware + software robotics project and contributions of all kinds are welcome.

---

## Ways to Contribute

- **Bug reports** — If a sketch behaves unexpectedly, open an issue with a clear description, the sketch name, your sensor readings, and the error (if any).
- **Improvements** — Better PID tuning values, improved calibration routines, or new control strategies are always appreciated.
- **Hardware variants** — Adaptations for different sensor arrays or motor drivers.
- **Documentation** — Clearer wiring guides, translated READMEs, or tutorial write-ups.

---

## Getting Started

1. **Fork** the repository and clone it locally.
2. Create a branch with a descriptive name:
   ```bash
   git checkout -b feature/improved-pid-tuning
   ```
3. Make your changes.
4. Commit with a clear message:
   ```bash
   git commit -m "feat: improve derivative low-pass filter for smoother turns"
   ```
5. Push and open a **Pull Request** against `main`.

---

## Code Style Guide

- Use **clear variable names** — avoid single-letter names except loop counters.
- Add **section comments** for PID, sensor, and motor logic blocks.
- Keep pin assignments at the **top of the file** as named constants.
- Include **Serial debug output** that can be disabled with a `#define DEBUG` flag.
- Test on actual hardware before submitting.

---

## Sketch File Naming

| Purpose | File |
|---|---|
| Primary flight sketch (production) | `src/main/pid8_code.ino` |
| Alternative PID with LPF | `src/main/lineFollowerPIDimplementation.ino` |
| Decision-maker variant | `src/main/decisionMakerLineFollower.ino` |
| Calibration utility | `src/calibration/BFDcalibration.ino` |
| Hardware tests | `src/tests/` |

---

## Issues

Please use the GitHub Issues tracker. For hardware questions, include:
- Your exact component versions
- Wiring photos if possible
- Serial monitor output

---

## License

By contributing, you agree that your contributions will be licensed under the project's [MIT License](LICENSE).
