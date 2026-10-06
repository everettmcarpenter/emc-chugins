@import "Rec.ck"

// instantiate a Henon
Henon obj => dac;

obj.alpha( 1.0 );
obj.beta( 0.8 );

Rec.auto();

10::second => now; 
