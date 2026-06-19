Grain test => Gain vol( 0.5 ) => dac;

Hid key; // hid
HidMsg msg; // hid decrypt
// device #
0 => int device;
// open keyboard ( get device number from command line )
if( !key.openKeyboard( device ) ) me.exit();
cherr <= "keyboard '" <= key.name() <= "' ready" <= "" <= IO.newline();

test.openFile( "../include/blackpaint.wav" );

test.pitch( 1.0 );
test.size( 142.0 );
50::ms => now;
<<< test.fileSize() >>>;

fun void tick( UGen ref )
{
    while( true )
    {
        <<< ref.last() >>>;
        100::ms => now;
    }
}

// pork ~ tick( test );

while( true )
{
    key => now;
    while( key.recv( msg ) )
    {
        if( msg.isButtonDown() )
        {
            if( msg.key == 20 )
            {
                test.position( Math.clampf( test.position() - ( test.position() * 0.01 ) - 0.001 , 0, 1.0 ) );
                <<< "- position ", test.position() >>>;
            }

            else if( msg.key == 8 )
            {
                test.position( Math.clampf( test.position() + ( test.position() * 0.01 ) + 0.001 , 0, 1.0 ) );
                <<< "+ position ", test.position() >>>;
            }

            else if( msg.key == 22 )
            {
                test.size( Math.clampf( test.size() - ( test.size() * 0.1 ) - 0.1 , 0.1, 1000.0 ) );
                <<< "- grain size ", test.size() >>>;
            }

            else if( msg.key == 26 )
            {
                test.size( Math.clampf( test.size() + ( test.size() * 0.1 ) + 0.1 , 0.1, 1000.0 ) );
                <<< "+ grain size ", test.size() >>>;
            }

            else if( msg.key == 7 )
            {
            	test.pitch( Math.clampf( test.pitch() + ( test.pitch() * 0.1 ) + 0.01, 0.001, 6.0 ) );
            	<<< "+ pitch ", test.pitch() >>>;
            }

            else if ( msg.key == 4 )
            {
            	test.pitch( Math.clampf( test.pitch() - ( 0.01 + test.pitch() * 0.01 ), 0.001, 6.0 ) );
            	<<< "- pitch ", test.pitch() >>>;
            }
        }
    }
}
