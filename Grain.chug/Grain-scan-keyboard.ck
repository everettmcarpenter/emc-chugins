@import "Rec"

Grain test( "../include/808cymbal.wav" )[36] => Gain vol( 1.0 / ( test.size() / 4.0 ) )[16] => DelayA del[16] => Matrix16 mat => dac;

mat.identity();
mat => del;

Hid key; // hid
HidMsg msg; // hid decrypt
// device #
0 => int device;
// open keyboard ( get device number from command line )
if( !key.openKeyboard( device ) ) me.exit();
cherr <= "keyboard '" <= key.name() <= "' ready" <= "" <= IO.newline();

for( int i; i < del.size(); i++ ) del[i].set( (i+1)*4::ms, (1+(i+1)*4)::ms );

for( int i; i < test.size(); i++ )
{
    test[i].pitch( 1.0 );
    test[i].size( 10.0 );
    <<< test[i].fileSize() >>>;
}

fun void tick( UGen ref )
{
    while( true )
    {
        <<< ref.last() >>>;
        100::ms => now;
    }
}

fun void update( Matrix16 object )
{
    Math.random2( 1,3 ) => int mult;
    float phase;
    while( true )
    {
        (0.001 / 2.0) +=> phase;
        if( phase > 2.0 * pi ) 2.0 * pi -=> phase; 
        for( int i; i < 4; i++ ) 
        {
            for( int j; j < 4; j++ )
            {
                if( j == i ) object.entry( 0.8 * Math.cos( mult * phase ), i, j );
                else object.entry( 0.8 * Math.sin( mult * phase ), i, j );
            }
        }
        10::ms => now;
    }
}

// pork ~ tick( test );
spork ~ update( mat );
spork ~ Rec.auto();

while( true )
{
    key => now;
    while( key.recv( msg ) )
    {
        if( msg.isButtonDown() )
        {
            if( msg.key == 20 )
            {
                for( int i; i < test.size(); i++ )
                {
                    test[i].position( Math.clampf( test[i].position() - ( test[i].position() * 0.01 ) - 0.001 , 0, 1.0 ) );
                    <<< "- position ", test[i].position() >>>;
                }
            }

            else if( msg.key == 8 )
            {
                for( int i; i < test.size(); i++ )
                {
                    test[i].position( Math.clampf( test[i].position() + ( test[i].position() * 0.01 ) + 0.001 , 0, 1.0 ) );
                    <<< "+ position ", test[i].position() >>>;
                }
            }

            else if( msg.key == 22 )
            {
                for( int i; i < test.size(); i++ )
                {
                    test[i].size( Math.clampf( test[i].size() + ( 0.1 + test[i].size() * 0.1 ), 0.001, 1200.0 ) );
                    <<< "- grain size ", test[i].size() >>>;
                }
            }

            else if( msg.key == 26 )
            {
                for( int i; i < test.size(); i++ )
                {
                    test[i].size( Math.clampf( test[i].size() - ( 0.1 + test[i].size() * 0.1 ), 0.001, 1200.0 ) );
                    <<< "+ grain size ", test[i].size() >>>;
                }
            }

            else if( msg.key == 7 )
            {
                for( int i; i < test.size(); i++ )
                {
            	    test[i].pitch( Math.clampf( test[i].pitch() + ( test[i].pitch() * 0.1 ) + 0.01, 0.001, 6.0 ) );
                    <<< "+ pitch ", test[i].pitch() >>>;
                }
            }

            else if ( msg.key == 4 )
            {
                for( int i; i < test.size(); i++ )
                {
            	    test[i].pitch( Math.clampf( test[i].pitch() - ( 0.01 + test[i].pitch() * 0.01 ), 0.001, 6.0 ) );
                    <<< "- pitch ", test[i].pitch() >>>;
                }
            }

            else if ( msg.key == 40 )
            {
                for( int i; i < test.size(); i++ )
                {
                    test[i].randomPitch( Math.clampf( test[i].randomPitch() + ( 0.01 + test[i].randomPitch() * 0.1 ), 0.001, 12.0 ) );
                    <<< "+ randomness of pitch ", test[i].randomPitch() >>>;
                }
            }

            else if ( msg.key == 229 )
            {
                for( int i; i < test.size(); i++ )
                {
                    test[i].randomPitch( Math.clampf( test[i].randomPitch() - ( 0.01 + test[i].randomPitch() * 0.1 ), 0.001, 12.0 ) );
                    <<< "- randomness of pitch ", test[i].randomPitch() >>>;
                }
            }

            else if ( msg.key == 228 )
            {
                for( int i; i < test.size(); i++ )
                {
                    test[i].randomSize( Math.clampf( test[i].randomSize() + ( 0.01 + test[i].randomSize() * 0.1 ), 0.001, 1200.0 ) );
                    <<< "+ randomness of grain size ", test[i].randomSize() >>>;
                }
            }

            else if ( msg.key == 230 )
            {
                for( int i; i < test.size(); i++ )
                {
                    // test[i].randomSize( Math.random2f( 0, 2000 ) );
                    test[i].randomSize( Math.clampf( test[i].randomSize() - ( 0.01 + test[i].randomSize() * 0.1 ), 0.001, 1200.0 ) );
                    <<< "- randomness of grain size ", test[i].randomSize() >>>;
                }
            }
            
            else if ( msg.key == 41 )
            {
                me.exit();
            }
        }
    }
}
