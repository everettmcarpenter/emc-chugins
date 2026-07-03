@import "GranulatorSupport.ck"

// instantiation
Hid hi;
HidMsg msg;
GranularSupport helper(1);
Granulator grains( "../include/251391__kwahmah_02__man-speaking-in-arabic-am.wav", 128 ) => dac;
// identify yourself
string address;
int port;
// device #
0 => int device;
// open keyboard (get device number from command line)
if( !hi.openKeyboard( device ) ) me.exit();
cherr <= "keyboard '" <= hi.name() <= "' ready" <= "" <= IO.newline();

while( true )
{
    hi => now;
    while( hi.recv( msg ) )
    {
        if( msg.isButtonDown() )
        {
            if( msg.ascii == 27 )
            {
                cherr <= "exiting";
                300::ms => now;
                cherr <= " . ";
                300::ms => now;
                cherr <= " . ";
                300::ms => now;
                cherr <= " . " <= IO.newline();
                me.exit();
            }
            else helper.key( msg.key, grains );
        }
    }
    10::ms => now;
}
