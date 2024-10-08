/*
 * Copyright (C) 2024 Arthur Zhang <arthur.zhang.cx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "base/base_def.h"
#include <QObject>

namespace PspNsBase {
    BASE_SHARED double PressureKPa2Pa(double KPa) {
        return KPa * 1.0E3;
    }

    BASE_SHARED double PressurePa2KPa(double pa) {
        return pa / 1.0E3;
    }

    BASE_SHARED double PressureConvert(double in, eUnitPressure inType, eUnitPressure outType) {
        if (inType == outType) return in;

        if (inType == eUnitPressure::ePa) {
            switch (outType) {
                case eUnitPressure::eKPa:
                    return in / 1.0E3;
                case eUnitPressure::eBar:
                    return in / 1.0E5;
                case eUnitPressure::eMPa:
                    return in / 1.0E6;
                case eUnitPressure::eGPa:
                    return in / 1.0E9;
                case eUnitPressure::eAtm:
                    return in / 101325.0;
                default:
                    assert(false);
                    return 0.0;
            }
        }

        if (inType == eUnitPressure::eKPa) {
            switch (outType) {
                case eUnitPressure::ePa:
                    return in * 1.0E3;
                case eUnitPressure::eBar:
                    return in / 1.0E2;
                case eUnitPressure::eMPa:
                    return in / 1.0E3;
                case eUnitPressure::eGPa:
                    return in / 1.0E6;
                case eUnitPressure::eAtm:
                    return in / 101.325;
                default:
                    assert(false);
                    return 0.0;
            }
        }

        if (inType == eUnitPressure::eBar) {
            switch (outType) {
                case eUnitPressure::ePa:
                    return in * 1.0E5;
                case eUnitPressure::eKPa:
                    return in * 1.0E2;
                case eUnitPressure::eMPa:
                    return in / 10.0;
                case eUnitPressure::eGPa:
                    return in / 1.0E4;
                case eUnitPressure::eAtm:
                    return in / 1.01325;
                default:
                    assert(false);
                    return 0.0;
            }
        }

        if (inType == eUnitPressure::eMPa) {
            switch (outType) {
                case eUnitPressure::ePa:
                    return in * 1.0E6;
                case eUnitPressure::eKPa:
                    return in * 1.0E3;
                case eUnitPressure::eBar:
                    return in * 10.0;
                case eUnitPressure::eGPa:
                    return in / 1.0E3;
                case eUnitPressure::eAtm:
                    return in / 0.101325;
                default:
                    assert(false);
                    return 0.0;
            }
        }

        if (inType == eUnitPressure::eGPa) {
            switch (outType) {
                case eUnitPressure::ePa:
                    return in * 1.0E9;
                case eUnitPressure::eKPa:
                    return in * 1.0E6;
                case eUnitPressure::eBar:
                    return in * 1.0E4;
                case eUnitPressure::eMPa:
                    return in * 1.0E3;
                case eUnitPressure::eAtm:
                    return in / 1.01325E-4;
                default:
                    assert(false);
                    return 0.0;
            }
        }

        if (inType == eUnitPressure::eAtm) {
            switch (outType) {
                case eUnitPressure::ePa:
                    return in / 9.8692e-6;
                case eUnitPressure::eKPa:
                    return in / 9.8692E-3;
                case eUnitPressure::eBar:
                    return in / 0.9869233;
                case eUnitPressure::eMPa:
                    return in / 9.8692327;
                case eUnitPressure::eGPa:
                    return in / 9.8692327E3;
                default:
                    assert(false);
                    return 0.0;
            }
        }

        assert(false);
        return 0.0;
    }

    BASE_SHARED QString PressureToString(eUnitPressure type) {
        switch (type) {
            case eUnitPressure::ePa:
                return QString("Pa");
            case eUnitPressure::eKPa:
                return QString("kPa");
            case eUnitPressure::eBar:
                return QString("Bar");
            case eUnitPressure::eMPa:
                return QString("MPa");
            case eUnitPressure::eGPa:
                return QString("GPa");
            case eUnitPressure::eAtm:
                return QString("atm");
            default:
                assert(false);
                return QString("");
        }

        assert(false);
        return QString("");
    }
    BASE_SHARED eUnitPressure PressureFromString(QString s) {
        if (s == QString("Pa")) return eUnitPressure::ePa;
        if (s == QString("kPa")) return eUnitPressure::eKPa;
        if (s == QString("Bar")) return eUnitPressure::eBar;
        if (s == QString("MPa")) return eUnitPressure::eMPa;
        if (s == QString("GPa")) return eUnitPressure::eGPa;
        if (s == QString("atm")) return eUnitPressure::eAtm;

        assert(false);
        return eUnitPressure::ePa;
    }

    BASE_SHARED double TemperatureC2K(double c) {
        return c + 273.15;
    }

    BASE_SHARED double TemperatureK2C(double k) {
        return k - 273.15;
    }

    BASE_SHARED double TemperatureConvert(double in, eUnitTemperature inType, eUnitTemperature outType) {
        if (inType == outType) return in;

        if (inType == eUnitTemperature::eK) {
            switch (outType) {
                case eUnitTemperature::eC:
                    return in - 273.15;
                case eUnitTemperature::eF:
                    return (in - 273.15) * 9.0 / 5.0 + 32.0;
                default:
                    assert(false);
            }
        }

        if (inType == eUnitTemperature::eC) {
            switch (outType) {
                case eUnitTemperature::eK:
                    return in + 273.15;
                case eUnitTemperature::eF:
                    return in * 9.0 / 5.0 + 32.0;
                default:
                    assert(false);
            }
        }

        if (inType == eUnitTemperature::eF) {
            switch (outType) {
                case eUnitTemperature::eK:
                    return 273.15 + (in - 32.0) * 5.0 / 9.0;
                case eUnitTemperature::eC:
                    return (in - 32.0) * 5.0 / 9.0;
                default:
                    assert(false);
            }
        }

        assert(false);
        return -10000.0;
    }

    BASE_SHARED QString TemperatureToString(eUnitTemperature type) {
        switch (type) {
            case eUnitTemperature::eK:
                return QString("K");
            case eUnitTemperature::eC:
                return QString("C");
            case eUnitTemperature::eF:
                return QString("F");
            default:
                assert(false);
        }

        assert(false);
        return QString("");
    }

    BASE_SHARED eUnitTemperature TemperatureFromString(QString s) {
        if (s == QString("K")) {
            return eUnitTemperature::eK;
        } else if (s == QString("C")) {
            return eUnitTemperature::eC;
        } else if (s == QString("F")) {
            return eUnitTemperature::eF;
        } else {
            assert(false);
            return eUnitTemperature::eK;
        }
    }

    BASE_SHARED QString TypeFormulaToString(eTypeFormula type) {
        switch ((int) type) {
            case (int) eTypeFormula::eLinear: {
                return QString(QObject::tr("Linear"));
            }
            case (int) eTypeFormula::eQuadratic: {
                return QString(QObject::tr("Quadratic"));
            }
            case (int) eTypeFormula::eCubic: {
                return QString(QObject::tr("Cubic"));
            }
            default: {
                return QString(QObject::tr("Quadratic"));
            }
        }

        assert(false);
        return QString(QObject::tr("Quadratic"));
    }

    BASE_SHARED eTypeFormula TypeFormulaFromString(QString s) {
        if (s == QString(QObject::tr("Linear"))) {
            return eTypeFormula::eLinear;
        } else if (s == QString(QObject::tr("Quadratic"))) {
            return eTypeFormula::eQuadratic;
        } else if (s == QString(QObject::tr("Cubic"))) {
            return eTypeFormula::eCubic;
        } else {
            return eTypeFormula::eQuadratic;
        }

        assert(false);
        return eTypeFormula::eQuadratic;
    }

}
