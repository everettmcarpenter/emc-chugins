public class GranularSupport // carbon copy of keyboard mapping from Everett
{
    int print;

    fun void GranularSupport() {};
    fun void GranularSupport( int print ) { print => this.print; };

    fun void key(int key, Granulator gran) // huge interface layer 
    {
        // position setting via numerics
        if( key < 40 && key > 29 )
        {
            ((key - 29)*gran.samples()/(10.0)) $ int => gran.position;
            if( print ) <<< "position: ", gran.position() >>>;
        }
        // enable spacer via alt key
        else if( key == 226 )
        {
            // (gran.spacer + 1) % 2 => gran.spacer;
            // if( print ) <<< "spacer: ", gran.spacer >>>;
        }
        // go to beginning of the file via `
        else if( key == 53 )
        {
            0 => gran.position;
            if( print ) <<< "position: ", gran.position() >>>;
        }
        // advance via = 
        else if( key == 46 )
        {
            Math.min(gran.samples(), ( gran.position() * gran.samples() ) + 11000) $ int => gran.position;
            if( print ) <<< "position: ", gran.position() >>>;
        }
        // and step back via -
        else if( key == 45 )
        {
            Math.max(1, ( gran.position() * gran.samples() ) - 11000) $ int => gran.position;
            if( print ) <<< "position: ", gran.position() >>>;
        }
        // random grain duration
        else if( key == 229 )
        {
            // shift to decrease random grain duration
            Math.max(0.01, ( gran.randomSize() / 1.3 )) => gran.randomSize;
            if( gran.randomSize() <= 0.01 ) 0.01 => gran.randomSize;
            if( print ) <<< "- randomness grain length: ", gran.randomSize() >>>;
        }
        else if( key == 40 )
        {
            if( gran.randomSize() <= 0.01 ) 0.01 => gran.randomSize;
            Math.min( 5000.0, (gran.randomSize() * 1.3 )) => gran.randomSize;
            if( print ) <<< "+ randomness grain length: ", gran.randomSize() >>>;
        }
        // reduce rand position via [
        else if( key == 47 )
        {
            (Math.max(0.0, gran.randomPosition() - 500.0)) $ int => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        // increase rand position via ]
        else if( key == 48 )
        {
            (Math.min(gran.samples(), gran.randomPosition() + 500)) $ int => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        // set random position via qwertyuiop
        else if( key == 20 )
        {
            0 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 26 )
        {
            200 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 8 )
        {
            2000 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 21 )
        {
            20000 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 23 )
        {
            40000 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 28 )
        {
            80000 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 24 )
        {
            100000 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 12 )
        {
            gran.samples() * 7 / 9 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 18 )
        {
            gran.samples() * 8 / 9 => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        else if( key == 19 )
        {
            gran.samples() => gran.randomPosition;
            if( print ) <<< "randomness of position: ", gran.randomPosition() >>>;
        }
        // pitch of granulator via asdfghjkl;' 
        else if( key == 10 )
        {
            1.0 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 9 )
        {
            0.75 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 7 )
        {
            0.5 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 22 )
        {
            0.25 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 4 )
        {
            0.000083 => gran.pitch; // 4 samples() per second at 48000
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 11 )
        {
            2.0 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 13 )
        {
            4.0 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 14 )
        {
            8.0 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 15 )
        {
            16.0 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 51 )
        {
            gran.pitch() - 0.05 / 12 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        else if( key == 52 )
        {
            gran.pitch() + 0.05 / 12 => gran.pitch;
            if( print ) <<< "pitch: ", gran.pitch() >>>;
        }
        // rand pitch via < and >
        else if( key == 54 )
        {
            gran.randomPitch() - 0.025 => gran.randomPitch;
            if( print ) <<< "rando of pitch: ", gran.randomPitch() >>>;
        }
        else if( key == 55 )
        {
            gran.randomPitch() + 0.025 => gran.randomPitch;
            if( print ) <<< "rando of pitch: ", gran.randomPitch() >>>;
        }
        // random pitch via zxcvbnm
        else if( key == 29 )
        {
            0.0 => gran.randomPitch;      
            if( print ) <<< "randomness of pitch: ", gran.randomPitch() >>>;
        }
        else if( key == 27 )
        {
            1.0 => gran.randomPitch;
            if( print ) <<< "randomness of pitch: ", gran.randomPitch() >>>;
        }
        else if( key == 6 )
        {
            2.0 => gran.randomPitch;
            if( print ) <<< "randomness of pitch: ", gran.randomPitch() >>>;
        }
        else if( key == 25 )
        {
            3.0 => gran.randomPitch;
            if( print ) <<< "randomness of pitch: ", gran.randomPitch() >>>;
        }
        else if( key == 5 )
        {
            4.0 => gran.randomPitch;
            if( print ) <<< "randomness of pitch: ", gran.randomPitch() >>>;
        }
        else if( key == 17 )
        {
            5.0 => gran.randomPitch;
            if( print ) <<< "randomness of pitch: ", gran.randomPitch() >>>;
        }
        else if( key == 16 )
        {
            6.0 => gran.randomPitch;
            if( print ) <<< "randomness of pitch: ", gran.randomPitch() >>>;
        }
        else if( key == 228 )
        {
            gran.size( Math.clampf( gran.size() + ((gran.size() / 6.0) + 0.01), 1.0, 5000.0 ) );
            if( print ) <<< "+ grain size: ", gran.size() >>>;
        }
        else if( key == 230 )
        {
            gran.size( Math.clampf( gran.size() - ((gran.size() / 6.0) + 0.01), 1.0, 5000.0 ) );
            if( print ) <<< "- grain size: ", gran.size() >>>;
        }
    }

    fun void mouse(float placement[], Granulator gran) // adjust grain duration and volume with mouse
    {
        ((placement[0] * (850.0 - 1.0) + 1.0)) => gran.size;
    }
}
