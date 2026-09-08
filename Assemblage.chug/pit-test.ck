@import "Line"

Assemblage grain("../include/Leeds-Bells.wav", 16) => Gain vol( 1.5 ) => NRev rev( 0.05 ) => dac;

Line grainSize => blackhole; 

grain.size( 25.0 );
grain.randomSize( 5.0 );

spork ~ tether( grain, grainSize, "size" );

grain.position( Math.randomf() );

5::second => now;

grain.pitch( [ 10.5, 4.0, 43.0, 2.0, 0.9 ], [ 5::second, 20::second, 1::second, 800::ms, 45::second ] );

grainSize.set( [ 0.8, 0.015, 0.001, 0.49, 0.032, 1.0 ], [ grain.duration() / 6.0, grain.duration() / 6.0, grain.duration() / 6.0, grain.duration() / 6.0, grain.duration() / 6.0, grain.duration() / 6.0 ] );
grainSize.keyOn();

45::second => now;

grain.pitch( [ 0.75, 2.0 ], [ 30::second, 45::second ] );

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
