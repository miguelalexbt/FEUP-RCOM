#ifndef LL_TEST_H
#define LL_TEST_H

//#define LL_TEST

#ifdef LL_TEST

    /** Propagation time (careful with timeout) */
    #define T_PROP      200

    /** Header's byte error ratio (use small number, ex: 10⁻²) */
    #define HEADER_BER  0.05

    /** Data's byte error ratio (use small numbers, ex: 10⁻²) */
    #define DATA_BER    0.05

    /**
    * Start test.
    */
    void start_test();

    /**
    * End test.
    */
    void end_test();

#endif

#endif