@import "Line"

Assemblage grain("../include/Rehearsal-Hannah-Izishaki.wav", 16) => NRev rev( 0.05 ) => dac;

Line grainSize => blackhole; 

grain.size( 25.0 );
grain.randomSize( 5.0 );

spork ~ tether( grain, grainSize, "size" );

grain.pitch( [ 1.0001, 1.0, 0.9999 ] );

grain.position( 1.0, grain.duration() );

grainSize.set( [ 0.015, 0.089, 0.032, 1.0 ], [ grain.duration() / 4.0, grain.duration() / 4.0, grain.duration() / 4.0, grain.duration() / 4.0 ] );
grainSize.keyOn();
grain.duration() => now;

fun void tether( Assemblage @ subject, Line @ interpolator, string parameter )
{
    while( true )
    {
        if( parameter == "size" )
        {
            subject.size( interpolator.last() * 1000.0 );
        }
        else if( parameter == "pitch" )
        {
            subject.pitch( interpolator.last() );
        }
        else if( parameter == "position" )
        {
            subject.position( interpolator.last() );
        }
        else if( parameter == "spacer" )
        {
            subject.spacer( interpolator.last()::ms );
        }
        else { cherr <= "Parameter non-existent"; me.exit(); }
        1::samp => now;
    }
}
