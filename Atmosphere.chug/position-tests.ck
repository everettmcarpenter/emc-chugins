Atmosphere2 atmos( "yuo.wav", 1 ) => blackhole;

atmos.position( 1.0, atmos.duration() * 2.0 ); // progress to end of file at half the recorded speed

while( atmos.position() != atmos.targetPosition() )
{
	<<< atmos.position(), atmos.targetPosition() >>>;
	100::ms => now;
} 
