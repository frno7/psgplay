// SPDX-License-Identifier: GPL-2.0

#include "atari/dac.h"

/*
 * Build a circuit analysed version of the conversion table. David Savinkoff
 * designed this algorithm by analysing data measured by Paulo Simoes and
 * Benjamin Gerard.
 *
 * The numbers are arrived at by assuming a current steering resistor ladder
 * network and using the voltage divider rule.
 *
 * If one looks at the ST schematic of the YM2149, one sees three sound pins
 * tied together and attached to a 1000 ohm resistor (1k) that has the other
 * end grounded.
 *
 * The 1k resistor is also in parallel with a 0.1 microfarad capacitor
 * (on the Atari ST, not STE or others). The voltage developed across the
 * 1K resistor is the output voltage which is called Vout.
 *
 * The output of the YM2149 is modelled well as pullup resistors. Thus,
 * the three sound pins are seen as three parallel computer-controlled,
 * adjustable pull-up resistors.
 *
 * To emulate the output of the YM2149, one must determine the resistance
 * values of the YM2149 relative to the 1k resistor, which is done by the
 * model.
 *
 * The AC + DC math model is
 *
 * (MaxVol*WARP) / (1.0 + 1.0/(conductance[i]+conductance[j]+conductance[k]))
 *
 * or
 *
 * (MaxVol*WARP) / (1.0 + 1.0/( 1/Ra +1/Rb  +1/Rc ))
 *
 * where Ra = channel A resistance, etc.
 *
 * Note that the first 1.0 in the formula represents the normalized
 * 1k resistor (1.0 * 1000 ohms = 1k).
 *
 * The YM2149 DC component model represents the output voltage filtered of
 * high frequency AC component, but DC component remains.
 *
 * The YM2149 DC component mode treats the voltage exactly as if it were
 * low pass filtered. This is more than what is required to make 'quartet
 * mode sound'. Simplicity leads to generality!
 *
 * The DC component model model is
 *
 * (MaxVol*WARP) / (2.0 + 1.0/( 1/Ra + 1/Rb  + 1/Rc))
 *
 * or
 *
 * (MaxVol*WARP*0.5) / (1.0 + 0.5/( 1/Ra + 1/Rb  + 1/Rc)).
 *
 * Note that 1.0 represents the normalized 1k resistor. 0.5 represents
 * 50 % duty cycle for the parallel resistors being summed (this effectively
 * doubles the pull-up resistance).
 */

struct conductance {
	double lvl[32];
};

static const double MAXVOL = 65119.0;	/* Normal mode maximum value in table. */
static const double FOURTH2 = 1.19;	/* Fourth root of two from YM2149. */
static const double WARP = 5.0 / 3.0;	/* Measured as 1.65932 from 46602. */

static struct conductance generate_conductance()
{
	struct conductance c = { .lvl[0] = 1.0e-8 /* Avoid divide by zero */ };

	/*
	 * YM2149 and R8=1k follows (2^-1/4)^(n-31) better when 2 voices are
	 * summed (A+B or B+C or C+A), rather than individually (A or B or C)
	 * when taking three voices into consideration.
	 *
	 * Note that the YM2149 does not have laser trimmed resistances, thus
	 * offsets are added and/or multiplied with (2^-1/4)^(n-31).
	 */
	double cc = 2.0 / 3.0 / (1.0 - 1.0 / WARP) - 2.0 / 3.0; /* conductance = 1.0 */

	/*
	 * The YM current output (voltage source with series resistances)
	 * is connected to a grounded resistor to develop the output voltage
	 * (instead of a current to voltage converter). The output transfer
	 * function is not linear. Thus:
	 *
	 * 2.0 * c[n] = 1.0 / (1.0 - 1.0 / FOURTH2 / (1.0 / cc + 1.0)) - 1.0
	 */
	for (int i = 31; i >= 1; i--) {
		c.lvl[i] = cc / 2.0;

		cc = 1.0 / (1.0 - 1.0 / FOURTH2 / (1.0 / cc + 1.0)) - 1.0;
	}

	return c;
}

static void generate_dac(struct cf2149_dac *dac)
{
	const struct conductance c = generate_conductance();

	/*
	 * Note that MAXVOL is 65119 in Simoes' table, 65535 in Gerard's.
	 *
	 * YM2149 AC + DC components model: Sum the conductances as a function
	 * of a voltage divider:
	 *
	 * Vout = Vin * Rout / (Rout + Rin)
	 */
	for (int i = 0; i < 32; i++)
	for (int j = 0; j < 32; j++)
	for (int k = 0; k < 32; k++)
		dac->lvl[i][j][k] = (int16_t)(0.5 + (MAXVOL * WARP) /
					(1.0 + 1.0 / (c.lvl[i] +
						      c.lvl[j] +
						      c.lvl[k])));

	/*
	 * Note that MAXVOL is 46602 in Paulo Simoes Quartet mode table.
	 *
	 * YM2149 DC component model: R8=1k (pulldown) + YM//1K (pullup)
	 * with YM 50 % duty PWM
	 *
	 *	for (i = 0; i < 32; i++)
	 *	for (j = 0; j < 32; j++)
	 *	for (k = 0; k < 32; k++) {
	 *		dac->lvl[i][j][k] = (int16_t)(0.5 + (MaxVol * WARP) /
	 *				(1.0 + 2.0 / (c.lvl[i] +
	 *					      c.lvl[j] +
	 *					      c.lvl[k])));
	 *	}
	 */
}

const struct cf2149_dac *cf2149_atari_st_dac()
{
	static bool init = false;
	static struct cf2149_dac dac;

	if (!init) {
		generate_dac(&dac);
		init = true;
	}

	return &dac;
}
