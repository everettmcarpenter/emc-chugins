Assemblage grain("../include/resolution.wav") => dac;

grain.size( 45.0 );

grain.spacer( 1::second );

5::second => now;

grain.spacer( 0::samp );

5::second => now;

grain.spacer( 500::second );
