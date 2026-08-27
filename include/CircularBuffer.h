//-----------------------------------------------------------------------------
// name: class CBufferSimple
// desc: circular buffer - one reader one writer ( stolen from chuck! )
//-----------------------------------------------------------------------------
class CBufferSimple
{
	CBufferSimple::CBufferSimple()
	{
	    m_data = NULL;
	    m_data_width = m_read_offset = m_write_offset = m_max_elem = 0;
	}

	CBufferSimple::~CBufferSimple()
	{
	    this->cleanup();
	}

	bool CBufferSimple::initialize( unsigned int num_elem, unsigned int width )
	{
	    // cleanup
	    cleanup();

	    // allocate
	    m_data = (double *)malloc( num_elem * width );
	    if( !m_data )
	        return false;

	    m_data_width = width;
	    m_read_offset = 0;
	    m_write_offset = 0;
	    m_max_elem = num_elem;

	    return true;
	}

	void CBufferSimple::cleanup()
	{
	    if( !m_data )
	        return;

	    free( m_data );

	    m_data = NULL;
	    m_data_width = m_read_offset = m_write_offset = m_max_elem = 0;
	}

	void CBufferSimple::put( void * data, unsigned int num_elem )
	{
	    unsigned int i, j;
	    double * d = (double *)data;

	    // copy
	    for( i = 0; i < num_elem; i++ )
	    {
	        for( j = 0; j < m_data_width; j++ )
	        {
	            m_data[m_write_offset*m_data_width+j] = d[i*m_data_width+j];
	        }

	        // move the write
	        // Aug 2014 - spencer
	        // change to fully "atomic" increment+wrap
	        m_write_offset = (m_write_offset + 1) % m_max_elem;
	    }
   	}

	unsigned int CBufferSimple::get( void * data, unsigned int num_elem )
	{
	    unsigned int i, j;
	    double * d = (double *)data;

	    // read catch up with write
	    if( m_read_offset == m_write_offset )
	        return 0;

	    // copy
	    for( i = 0; i < num_elem; i++ )
	    {
	        for( j = 0; j < m_data_width; j++ )
	        {
	            d[i*m_data_width+j] = m_data[m_read_offset*m_data_width+j];
	        }

	        // move read
	        m_read_offset = (m_read_offset + 1) % m_max_elem;

	        // catch up
	        if( m_read_offset == m_write_offset )
	        {
	            i++;
	            break;
	        }
	    }
	    // return number of elems
	    return 1; // shouldn't it return i?
	}
protected:
    double* m_data;
    unsigned int m_data_width;
    unsigned int m_read_offset;
    unsigned int m_write_offset;
    unsigned int m_max_elem;
};
