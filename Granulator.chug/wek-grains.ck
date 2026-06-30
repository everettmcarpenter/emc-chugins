// instantiate a global Wekinator, so other files can access
Wekinator wek;

Granulator grains( "../include/randomvaporwave.wav", 16 ) => dac;

grains.pitch( 1.0 );

// clear training observations
wek.clear();

// HID input and HID message
Hid hi;
HidMsg msg;

float last[3];
float parameters[4];

// which joystick
0 => int device;
// get from command line
if( me.args() ) me.arg(0) => Std.atoi => device;

// open joystick 0, exit on fail
if( !hi.openJoystick( device ) ) me.exit();

<<< "joystick '" + hi.name() + "' ready", "" >>>;

spork ~ hid();

cherr <= "-----------" <= IO.newline();
// add a group of data
addGroup( wek, [0.0,0.0,0.0], [8.0, 0.5, 5.0, 0.0], 80 );
addGroup( wek, [-1.0,-1.0,0.0], [248.0, 0.0, 50.0, 0.0], 20 );
addGroup( wek, [-1.0,1.0,0.0], [196.0, 1.0, 20.0, 0.0], 20 );
addGroup( wek, [1.0,-1.0,0.0], [32.0, 0.0, 500.0, 0.0], 20 );
addGroup( wek, [1.0,1.0,0.0], [21.0, 1.0, 200.0, 0.0], 20 );
cherr <= "-----------" <= IO.newline();

// (optional) set model type; MLP is default
// AI.MLP => wek.modelType;
// (optional) set task type; regression is default
AI.Regression => wek.taskType;
// print
<<< "modelType:", wek.modelTypeName(),
    "| taskType:", wek.taskTypeName() >>>;

// (optional) set properties to your liking
// <<< "changing Wekinator MLP properties...", "" >>>;
wek.setProperty( AI.MLP, "hiddenLayers", 4 );
// note: 0 nodesPerHiddenLayer means "same as input layer"
wek.setProperty( AI.MLP, "nodesPerHiddenLayer", 4 );
// wek.setProperty( AI.MLP, "learningRate", 0.01 );
wek.setProperty( AI.MLP, "epochs", 500 );

 // print model properties
<<< "Wekinator MLP hiddenLayers:", wek.getPropertyInt( AI.MLP, "hiddenLayers" ) >>>;
<<< "Wekinator MLP nodesPerHiddenLayer:", wek.getPropertyInt( AI.MLP, "nodesPerHiddenLayer" ) >>>;
<<< "Wekinator MLP learningRate", wek.getPropertyFloat( AI.MLP, "learningRate" ) >>>;
<<< "Wekinator MLP epochs:", wek.getPropertyInt( AI.MLP, "epochs" ) >>>;
// (optional) save the observations for loading later
// wek.exportObs( me.dir() + "currentData.arff" );

// print
cherr <= "-----------" <= IO.newline();
cherr <= "training..." <= IO.newline();
cherr <= "-----------" <= IO.newline();
// train using current training set
wek.train();

while( true )
{
    10::ms => now;
}

// hid reader
fun void hid()
{
    // infinite event loop
    while( true )
    {
        // wait on HidIn as event
        hi => now;

        // messages received
        while( hi.recv( msg ) )
        {
            // joystick axis motion
            if( msg.isAxisMotion() )
            {
                //<<< "joystick axis", msg.which, ":", msg.axisPosition >>>;
                msg.axisPosition => last[msg.which];
            }
            
            // joystick button down
            else if( msg.isButtonDown() )
            {
                <<< "joystick button", msg.which, "down" >>>;
                if( msg.which == 0 )
                {
                    parameters.zero();
                    predict( wek, last, parameters );
                    grains.size( parameters[0] );
                    grains.position( parameters[1] * parameters[1] );
                    grains.randomSize( parameters[2] );
                    // grains.randomPosition( parameters[3] );
                    for( int i; i < parameters.size(); i++ ) cherr <= parameters[i] <= " ";
                    cherr <= IO.nl();
                }
            }
            
            // joystick button up
            else if( msg.isButtonUp() )
            {
                <<< "joystick button", msg.which, "up" >>>;
            }
            
            // joystick hat/POV switch/d-pad motion
            else if( msg.isHatMotion() )
            {
                <<< "joystick hat", msg.which, ":", msg.idata >>>;
            }
        }
    }
}

// add a group of training observations
fun void addGroup( Wekinator wek, float inputs[], float outputs[], int N )
{
    // collect data
    repeat( N )
    {
        // input and output (could be set in another file)
        wek.input( inputs ); wek.output( outputs );
        // add current input and output (typically these are different)
        wek.add();
    }
    // print
    cherr <= IO.newline();
}

// predict and print
fun void predict( Wekinator wek, float inputs[], float outputs[] )
{
    // predict output based on input; 3 inputs -> 2 outputs
    wek.predict(inputs, outputs);
}
