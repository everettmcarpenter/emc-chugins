@import "Line"

Assemblage grain("../include/resolution.wav") => blackhole;
Line grainSize(5::second) => blackhole;
Line grainPitch(5::second) => blackhole;
Line grainPosition(5::second) => blackhole;

// grainSize.help();

spork ~ linker();
// spork ~ printer();

while( true )
{
	grainSize.keyOn();
	grainPitch.keyOn();
	grainPosition.keyOn() => now;
	10::second => now;
	grainSize.keyOff();
	grainPitch.keyOff();
	grainPosition.keyOff() => now;	
}

fun void linker()
{
	while( true )
	{
		0.5 + ( grainPitch.last() * 1200.0 ) => grain.size;
		0.0125 + ( grainPitch.last() * 4.0 ) => grain.pitch;
		grainPosition.last() => grain.position;
		1::samp => now;
	}
}

fun void printer()
{
	while( true )
	{
		<<< grainSize.last(), grainPitch.last(), grainPosition.last() >>>;
		100::ms => now;
	}
}
