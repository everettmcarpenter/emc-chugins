Atmosphere2 atmos( 1 ) => blackhole;

1000::ms => now;

<<< "opening file. " >>>;
atmos.openFile( "yuo.wav" );

1000::ms => now;

<<< "closing file. " >>>;
atmos.closeFile();

1000::ms => now;
