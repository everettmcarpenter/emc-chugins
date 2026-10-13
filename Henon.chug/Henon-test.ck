@import "Rec.ck"

public class LFO
{
    1.0 => float freq; // normalized
    float increment; // step size
    float phase; // current val
    second / samp => float fs;
 
    fun @construct( float nFreq )
    {
        nFreq => freq;
        ( freq / fs ) * 2.0 * Math.pi => increment;
    }

    fun float tick() 
    {
        increment +=> phase;

        if( phase >= 2.0 * Math.pi )
            phase - ( 2.0 * Math.pi ) => phase;
        else if( phase <= -2.0 * Math.pi )
            phase + ( 2.0 * Math.pi ) => phase;

        return Math.sin( phase );
    }
}

public class Comb extends Chugraph
{
	inlet => outlet;
	inlet => DelayA del( 20::ms ) => Gain feedback( 0.9 ) => inlet;

	fun void feedgain( float gain ) { feedback.gain( gain ); }
	fun void delay( dur delay ) { del.delay( delay ); }
}

// instantiate a Henon
Henon obj[2] => Gain directSound( 1.0 )[2] => dac;
obj => Gain filterSound( 1.0 - directSound[0].gain() )[2] => Comb combs[2] => NRev rev( 0.025 )[2] => dac;

LFO modA( 0.25 );
LFO modB( 0.005 );

Rec.auto();

while( true )
{
	modA.tick() * 1.6 + 0.9 => obj[0].alpha;
	modB.tick() * 1.0 + 0.5 => obj[0].beta;
	-1.0 * modA.tick() * 1.8 + 1.2 => obj[1].alpha;
	-1.0 * modB.tick() * 0.7 + 0.2 => obj[1].beta;
	1::samp => now;
}
