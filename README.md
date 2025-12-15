# 🚗 HOW TO NXP — NXP CUP 2025 🇹🇳

> Welcome to the official repository for our **NXP Cup 2025** journey — the first Tunisian team to participate internationally!

---

## 🎯 Introduction

L'compétition hethi ena nraha men a7sen l'fouras il wa7ed ykhdem 7aja professionnelle w internationale.

NXP Cup hiya autonomous lane-following robot competition based on camera + microcontroller Mch compétition s3iba barcha, ama tt3allem menha barcha 7ajet y3awnouk career-wise.

🇹🇳 Tunisia this year is joining for the second time. Haja behya barcha howa elli NXP t7ab tinvesti fi tounes wtjib akther teams — let's make it count.

---

## 📚 Documentation

For detailed hardware design documentation, mechanical considerations, and lessons learned from our competition experience, see [HARDWARE_DOCUMENTATION.md](HARDWARE_DOCUMENTATION.md).

---

## 🔧 Matériel Utilisé

| Component     | Description                          |
| ------------- | ------------------------------------ |
| 📷 Pixy2      | Lane-detection camera                |
| 🧠 Teensy 4.x | Microcontroller (Arduino-compatible) |

| ⚙️ JGA25-371 | 12V, 620RPM Motors with Encoders |
| 🔋 Lipo 3S | 3000 mAh Battery Pack |

> Teensy 4.x is widely used abroad. It's Arduino-friendly, fast, and advanced. L'équipe lbarra kolhom ist3mlouh.

---

## 🧪 Li t3allemnah / Machakil Encountered

- 🧱 Track fl final wl qualif LEGO (kima haka )
  - Robot lazmou mayokhrajch mel lane (otherwise errors accumulate)
- ⚙️ **Motors** (600 RPM): mch khayba hne fi tounes behi ama l bara fama aabed tekhdmo ala vitesse makhir
- 📅 **Time**: Important to start testing early — kima ay compétition, early testing = better results

### Pixy2 Camera:

- Mch khayba, ama needs time to get used to
- **Tuning w environment sensitivity** matters
- Yelzemk t3awd calibration 3la 7asab el lighting(settings fi wast l camera )

---

## 💡 Advices

### ⚙️ Mécanique:

- Matkhdhouch l design kima howa — adapt according to your robot
- Lbarra kol équipe andhom version wahdhom — **no one uses the Gitbook version blindly**

### ⚡ Electronique:

- amnewl amalna shield kima yaamlou l euro ama mamchech maana belgde l shield lazmo yetkhdem ala kaada kena tnajmo taamlou pcb khir barcha
- Shield ykoun modular, soldered and robust

### 📋 Organisation:

- Mel awal tfahmou: koll wahed chnwa chyekhdem
- **Communication** essential to avoid duplicate work or misalignment

---

## 🧠 Architecture Avancée (tendances lbara)

L’équipes avancées:

- Yest3mlou **Single Board Computers (Raspberry Pi)** + NXP chip
- Full **computer vision** pipelines
- Some even do **live visualization** of camera view + robot reaction (amazing tool during debugging)

> Hatta si ma t7ebch SBC, at least plan your robot with **debugging outputs** and **control override modules**

---

## 📻 Useful Features to Add

- **Radio override** (e.g., FS-IA6 module)
- Remote start / stop bouton
- Debug serial logging
- Visualization tools (live camera feedback if possible)

---

## 💰 Budget

| Item                 | Estimate (TND) / (EUR)          |
| -------------------- | ------------------------------- |
| Full robot (Tunisia) | ~600 TND                        |
| Final transport aid  | NXP gives ~200 EUR for travel   |
| Real cost (avg)      | 300–400 EUR for flight (return) |

> Booking late is expensive. Organize **qualification early** → visa processing + cheaper flights.

---

## 🌍 Final 2025?

Mazelt l location official ma t3rfnahach, **possible Italy 🇮🇹**:

- Visa processing easy
- Flights cheap
- Much better for Tunisia-based teams

---

## 🔗 Helpful Links

| Resource            | Link        |
| ------------------- | ----------- |
| Pixy2 Documentation | [link here] |
| Teensy 4.x Overview | [link here] |
| NXP Cup Gitbook     | [link here] |
| Example Final Track | [link here] |

_(Add links you use most often for setup, calibration, components, or inspiration.)_

---

## 🫡 Final Words

L'compétition hethi mch ghir project ta3 fac — heya **chance réelle** t'build a pro-grade robot, t7ell les portes le l'industrie w tech international.

Start early, debug well, and enjoy the ride.

**#TeamTunisia 🇹🇳 — NXP CUP 2025**

