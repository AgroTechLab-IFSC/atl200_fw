# Description

Pull request summary:
- problem/feature description:
- related issue:
- fixes # (issue):

## Type of Change:

Select the type of change:
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Performance/Energy Optimization
- [ ] Refactoring (no functional changes)

# Hardware & Firmware Context

Versions:
- target PCB:
- ESP-IDF:

Partition table changes:
- [ ] No changes
- [ ] Partition sizes updated (attach updated partitions.csv)

# Connectivity & Communication

Primary Interface:
- [ ] WiFi
- [ ] LoRaWAN (OTAA/ABP)
- [ ] LTE / NB-IoT

## Validation Details:

Mark as needed:
- [ ] MQTT Broker connection successful (with TLS 1.2/1.3)
- [ ] Payload format matches the cloud decoder (link to decoder PR if applicable)
- [ ] For LoRaWAN: Verified Spreading Factor (SF) and Duty Cycle compliance
- [ ] For NB-IoT: Verified PSM (Power Saving Mode) and eDRX settings

## Sensor Calibration & Data Integrity
- Sensors Modified/Added:
- Calibration Coefficients:
- Test Evidence:

## Power Consumption Report
- Deep-sleep Current: ____ uA (Measured)
- Peak Transmission Current: ____ mA (Measured)
- Estimated Battery Autonomy:

## Testing Checklist
- [ ] Unit Testing: All Unity-based tests passed (idf.py test).
- [ ] Functional Testing: Station correctly logs all parameters for at least 4 hours.
- [ ] OTA Verification: Firmware update via OTA (WiFi/4G) was successful and rollback is functional.
- [ ] Static Analysis: No new compiler warnings; Linter rules followed.
- [ ] Hardware-in-the-Loop (HIL): Verified on actual target hardware, not just a devkit.

## Final Checklist
- [ ] I have performed a self-review of my own code.
- [ ] I have commented my code, particularly in hard-to-understand areas (e.g., bitwise ops, complex math).
- [ ] I have made corresponding changes to the documentation (Wiki/README).
- [ ] My changes generate no new warnings.
- [ ] New and existing unit tests pass locally.
- [ ] Credentials (SSIDs, Keys) are managed via NVS or Kconfig, NOT hardcoded.