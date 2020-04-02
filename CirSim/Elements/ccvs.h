#ifndef _CCVS_H
#define _CCVS_H

#include "voltagesource.h"
#include "../Node/node.h"

class CCVS : public VoltageSource
{
private: //Members
	std::string m_controlElement;
	Complex m_currentFactor;

public: //Static Current controlled Voltage Source creation
	static CCVS* createCCVS(const std::string& ccvsName, Node& posNode, Node& negNode, Complex currentFactor, const std::string& controlElement, Complex internalImpedance);

private: //Constructors
	CCVS(const std::string& ccvsName, Node& posNode, Node& negNode, Complex currentFactor, const std::string& controlElement, Complex internalImpedance = 0);
	~CCVS();

public: //Matrix Operations
	void injectIntoMatrix(Complex* matrix, size_t matrixWidth, std::map<std::string, size_t>& nodeIndexMap, std::map<std::string, size_t>& voltageIndexMap, double angularFrequency = 0.0);
	void injectVSCurrentControlIntoMatrix(Complex* matrix, size_t matrixWidth, CCVS* ccvs, Complex totalCurrentFactor, std::map<std::string, size_t> nodeIndexMap, std::map<std::string, size_t> voltageIndexMap, double angularFrequency = 0.0);
	void injectCSCurrentControlIntoMatrix(Complex* matrix, size_t matrixWidth, CCCS* cccs, Complex totalCurrentFactor, std::map<std::string, size_t> nodeIndexMap, std::map<std::string, size_t> voltageIndexMap, double angularFrequency = 0.0);

public: //Setters
	inline void setCurrentFactor(Complex currentFactor) { m_currentFactor = currentFactor;  }
	inline void setControlElement(const std::string& controlElement) { m_controlElement = controlElement; }

private: //Blocked Setters
	inline void setSupplyVoltage(Complex supplyVoltage) { }

public: //Getters
	inline Element* getControlElement() const { Element* element = Element::getElement(m_controlElement); if (element == nullptr) throw std::runtime_error("CCVS: Couldn't find ControlElement"); return element; }
	inline Complex getCurrentFactor() const { return m_currentFactor; }
	Complex getControlCurrent(double angularFrequency) const;
	inline Complex getSupplyVoltage(double angularFrequency) const { return m_currentFactor * getControlCurrent(angularFrequency); }
	inline Complex getPowerSupplied(double angularFrequency) const { return m_current * getSupplyVoltage(angularFrequency); }
	inline Complex getPowerDissipated() const { return m_internalImpedance * getCurrent().getMagnitudeSqr(); }
	inline Complex getTotalPowerSupplied(double angularFrequency) const { return getPowerSupplied(angularFrequency) - getPowerDissipated(); }

private: //Blocked Getters
	inline Complex getSupplyVoltage() const { }
	inline Complex getPowerSupplied() const { }
	inline Complex getTotalPowerSupplied() const { }

	CCVS(const CCVS&) = delete;
	void operator=(const CCVS&) = delete;
};

#endif //_CCVS_H
