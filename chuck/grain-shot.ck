public class GrainShot extends Chugraph
{
    
    Assemblage grain => ADSR envelope => outlet;

    dur _fileLength; // how long is the file
    float _target;

    // travel distance length of the file at time_to rate
    fun dur keyOn( dur distance, dur time_to )
    {
        // [0.0, 1.0]
        float target; 
        // how big is the distance relative to our file length
        ( distance / _fileLength ) $ float => float offset; 
        // add our offset to the current position to find the new target we will be approaching
        this.setTarget( offset + grain.position() );
        // head towards the new target and take as long as you say
        grain.position( this.target(), time_to );
        // turn on the envelope ( the grain and envelope arent tethered to each other )
        envelope.keyOn();
        // for love 
        return time_to; 
    }

    // (de)overload if you don't want to move the grain
    fun dur keyOn()
    {
        // make some sound
        envelope.keyOn();
        // for love 
        return envelope.releaseTime();
    }

    // overload if you wanna move
    fun dur keyOff( dur distance, dur time_to )
    {
        // [0.0, 1.0]
        float target; 
        // how big is the distance relative to our file length
        ( distance / _fileLength ) $ float => float offset; 
        // add our offset to the current position to find the new target we will be approaching
        this.setTarget( offset + grain.position() );
        // head towards the new target and take as long as you say
        grain.position( this.target(), time_to );
        // turn off the envelope ( the grain and envelope arent tethered to each other )
        envelope.keyOff();
        // for love 
        return time_to; 
    }

    // key off
    fun dur keyOff()
    {
        // turn off envelope
        envelope.keyOff();
        // jump to the target if we haven't gotten there already
        // this uses the envelope release time
        grain.position( this.target(), envelope.releaseTime() );
        // for love
        return envelope.releaseTime();
    }

    fun void setTarget( float nextTarget )
    {
        if( nextTarget > 1.0 ) 1.0 => nextTarget; 
        nextTarget => _target;
    }

    fun float target()
    {
        return _target;
    }
}