// -----------------------------------------------------
//	
//	A collection of some interpolation methods
// 
//  lhs = past input
//  rhs = latest input
//  distance = fractional delay
//
// -----------------------------------------------------

// one multiply lerp as described by JOS!
// https://ccrma.stanford.edu/~jos/pasp/One_Multiply_Linear_Interpolation.html
template <typename T> 
T lerp( T lhs, T rhs, double distance )
{
    if( distance >= 0.0 && distance <= 1.0 )
    {
        return lhs + distance * ( rhs - lhs );
    }
    else
    {
        return rhs;
    }
}

// allpass interpolation
// https://ccrma.stanford.edu/~jos/pasp/First_Order_Allpass_Interpolation.html
template <typename T>
T allpass( T lhs, T rhs, T past, double distance )
{
    if( distance >= 0.0 && distance <= 1.0 )
    {
        return distance * ( rhs - past ) + lhs; 
    }
    else
    {
        return rhs;
    }
}