@import "Rec.ck"

Rec.auto();

Collage collage( "../include/Noise-Room.wav", 48 ) => NRev leftReverb( 0.125 )=> dac.chan( 0 );
Collage painting( "../include/Noise-Room.wav", 48 ) => NRev rightReverb( 0.125 ) => dac.chan( 1 );

leftReverb => DelayA delayLeft( 780::ms ) => Gain feedbackLeft( 0.1 ) => leftReverb;
rightReverb => DelayA delayRight( 780::ms ) => Gain feedbackRight( 0.1 ) => rightReverb;

collage.pitch( 0.0001 );
painting.pitch( 4.0 );

collage.size( 30.0 );
painting.size( 30.0 );

// collage.randomSize( 1.0 );
// painting.randomSize( 1.0 );
// collage.randomPosition( 50.0 );
// painting.randomPosition( 50.0 );

collage.position( 1.0, collage.duration() * 2.0 );
collage.pitch( 4.0, collage.duration() * 2.0 );

painting.position( 1.0, collage.duration() * 2.0 );
painting.pitch( 0.0001, collage.duration() * 2.0 );

collage.duration() * 2.5 => now;
