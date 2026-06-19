Grain test => Gain vol( 0.5 ) => dac;

test.openFile( "../include/blackpaint.wav" );

test.pitch( 1.0 );

<<< test.fileSize() >>>;

1::second => now;

<<< test.fileSize() >>>;

for( int i; i < test.fileSize(); i++ )
{
	test.size( ( i + 1 ) * 2000.0 );
	test.position( i );
	10::ms => now;
}
