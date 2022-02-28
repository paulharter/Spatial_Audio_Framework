/*
 * Copyright 2020-2021 Michael McCrea, Leo McCormack
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file test__dvf_module.c
 * @brief Unit tests for the SAF distance variation function (DVF) module for
 * binaural near field source filtering, as described in [1].
 *
 * @see [1] S. Spagnol, E. Tavazzi, and F. Avanzini, “Distance rendering and
 *          perception of nearby virtual sound sources with a near-field filter
 *          model,” Applied Acoustics, vol. 115, pp. 61–73, Jan. 2017,
 *          doi: 10.1016/j.apacoust.2016.08.015.
 *
 * @author Michael McCrea
 * @date 16.03.2021
 * @license ISC
 */

#include "saf_test.h"

/* Target values are generated by MATLAB functions in `generate_coeffs_for_plugin_tests.m`
 * (corresponding function names are noted above each data set), which is not included in this
 * repository but are in the author's development repository `nearfield_rangeextrapolation`.
 */

void test__dvf_calcHighShelfParams(void){
    
    /* setup */
    const float acceptedTolerance = 0.00001f;
    const float acceptedTolerance_fc = 0.1f;
    const int nTheta = 19;
    const int nRho = 5;
    const float rho[nRho] = {1.150000,1.250000,1.570000,2.381000,3.990000};
    const float g0_ref[nRho][nTheta] = { /* testRhoCoeffs_g_0 */
        {22.670282,17.717752,11.902597,7.906282,4.720884,2.217061,0.134088,-1.613730,-3.095960,-5.279052,-5.433653,-6.342905,-7.107677,-7.744796,-8.236084,-8.613662,-8.864276,-9.065870,-9.089385},
        {18.295933,15.510441,11.452312,7.951083,4.997235,2.609075,0.592613,-1.107964,-2.557504,-4.547256,-4.853912,-5.750024,-6.504702,-7.133244,-7.621092,-7.993574,-8.244015,-8.438287,-8.467470},
        {11.937032,11.093339,9.245757,7.118216,4.990070,3.083402,1.371444,-0.121838,-1.427296,-2.979742,-3.542803,-4.381065,-5.091220,-5.683427,-6.149122,-6.508598,-6.748356,-6.923465,-6.961620},
        {6.676990,6.451424,5.818377,4.924700,3.861979,2.760683,1.662668,0.629080,-0.327831,-1.328149,-1.970549,-2.649238,-3.234743,-3.727775,-4.122829,-4.433178,-4.640902,-4.783351,-4.823625},
        {3.628860,3.534311,3.298166,2.922799,2.438587,1.888286,1.296135,0.698518,0.112899,-0.473626,-0.960644,-1.428032,-1.841763,-2.196404,-2.487131,-2.717121,-2.873915,-2.978235,-3.010937}
    };
    
    const float gInf_ref[nRho][nTheta] = { /* testRhoCoeffs_g_inf */
        {-4.643651,-4.225287,-4.134752,-4.386332,-5.244711,-6.439307,-7.659091,-8.887172,-10.004796,-10.694171,-11.190476,-10.876569,-10.140292,-9.913242,-9.411469,-8.981807,-8.723677,-8.529900,-8.574359},
        {-4.128221,-3.834507,-3.575000,-3.637788,-4.278932,-5.310000,-6.609705,-7.815000,-8.925450,-9.646588,-10.000000,-9.784733,-9.301643,-8.862963,-8.370815,-7.953778,-7.693305,-7.500645,-7.518260},
        {-3.094135,-2.963709,-2.721834,-2.573043,-2.793627,-3.414652,-4.403297,-5.518539,-6.578461,-7.332562,-7.702192,-7.582977,-7.376856,-6.745349,-6.279895,-5.891862,-5.636418,-5.456323,-5.437006},
        {-1.937289,-1.889079,-1.765709,-1.620800,-1.598110,-1.815613,-2.314443,-3.041183,-3.857777,-4.533446,-4.931544,-4.962571,-4.717069,-4.357935,-3.971281,-3.646312,-3.422461,-3.269044,-3.231471},
        {-1.126412,-1.103440,-1.049199,-0.969714,-0.917898,-0.962176,-1.182409,-1.566237,-2.065834,-2.552771,-2.884909,-2.977707,-2.811758,-2.629199,-2.355800,-2.118920,-1.949860,-1.834291,-1.800638}
    };
    const float fc_ref[nRho][nTheta] = { /* testRhoCoeffs_f_c */
        {525.636204,409.078426,427.552571,936.671283,1635.128987,2622.394035,3167.199181,3899.649293,4176.703569,4361.226917,4634.448076,4516.401848,4567.834168,4685.234222,4908.786495,4966.258562,4936.982049,4927.963688,5210.861482},
        {410.072475,389.319631,398.844102,613.394238,1116.223303,2095.651724,2847.557763,3726.141143,4080.406901,4304.960791,4463.911798,4449.375495,4501.166349,4623.582375,4757.884246,4911.093999,4935.074404,4940.266143,5155.085794},
        {358.247441,352.931439,352.752741,402.566754,590.733021,1127.131294,2007.589994,3160.896502,3808.131027,4155.246718,4336.853155,4375.553567,4406.656373,4543.636509,4649.878140,4849.374583,4974.986343,5006.214905,5164.504029},
        {318.842699,318.199637,315.776327,326.423309,364.498469,500.548368,980.626776,2174.301881,3296.777215,3904.656864,4203.152454,4329.347194,4338.652755,4492.976051,4579.879128,4849.678327,5052.801340,5116.753611,5267.402018},
        {297.454930,297.570719,296.701047,300.362959,308.255747,342.596563,509.934390,1379.970914,2702.827191,3646.599635,4078.866661,4301.570222,4303.807248,4472.223890,4535.654099,4855.399825,5119.558700,5210.329993,5380.750972}
    };
    
    /* params to test */
    float g0, gInf, fc;
    
    for(int ri = 0; ri < nRho; ri++){
        float r = rho[ri];
        for(int ti = 0; ti < nTheta; ti++){
            calcHighShelfParams(ti, r, &g0, &gInf, &fc);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance, g0_ref[ri][ti], g0);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance, gInf_ref[ri][ti], gInf);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance_fc, fc_ref[ri][ti], fc);
        }
    }
}

/* Parameter interpolation is implicitly checked in test__dvf_calcHighShelfCoeffs.
   A more granular test could just check the interpolation function. */
void test__dvf_interpHighShelfParams(void){
    
    /* interpHighShelfCoeffs() calls calcHighShelfParams() twice to generate the high
     shelf parameters for the nearests thetas in the lookup table. Those parameters
     are subsequently interpolated. So the success of interpHighShelfCoeffs() relies
     first on the calcHighShelfParams(), so that should be tested first. */
    
    /* setup */
    const float acceptedTolerance = 0.0001f;  // TODO: revisit these thresholds
    const float acceptedTolerance_frq = 0.01f;
    const int   nTheta = 6;
    const float theta[nTheta] = {0.000000,2.300000,47.614000,98.600000,166.200000,180.000000};
    const int   nRho = 5;
    const float rho[nRho] = {1.150000,1.250000,1.570000,2.381000,3.990000};
    const float iG0_ref[nRho][nTheta] = { /* testShelfParamsInterp_iG_0 */
        {22.670282,21.531200,2.814473,-5.412009,-8.989264,-9.089385},
        {18.295933,17.655270,3.178890,-4.810981,-8.364464,-8.467470},
        {11.937032,11.742982,3.538333,-3.463974,-6.856924,-6.961620},
        {6.676990,6.625110,3.023452,-1.880613,-4.729220,-4.823625},
        {3.628860,3.607114,2.019588,-0.892461,-2.938593,-3.010937}
    };
    const float iGInf_ref[nRho][nTheta] = { /* testShelfParamsInterp_iG_inf */
        {-4.643651,-4.547427,-6.154277,-11.120993,-8.603536,-8.574359},
        {-4.128221,-4.060667,-5.063987,-9.950522,-7.573856,-7.518260},
        {-3.094135,-3.064137,-3.266476,-7.650444,-5.524759,-5.437006},
        {-1.937289,-1.926201,-1.763717,-4.875811,-3.327342,-3.231471},
        {-1.126412,-1.121129,-0.951611,-2.838410,-1.878207,-1.800638}
    };
    const float iFc_ref[nRho][nTheta] = { /* testShelfParamsInterp_if_c */
        {525.636204,498.827915,2386.832594,4596.197114,4931.390665,5210.861482},
        {410.072475,405.299321,1861.960103,4441.658657,4938.293282,5155.085794},
        {358.247441,357.024760,999.146666,4311.428254,4994.348051,5164.504029},
        {318.842699,318.694795,468.086862,4161.363071,5092.451748,5267.402018},
        {297.454930,297.481562,334.402844,4018.349277,5175.836901,5380.750972}
    };
    /* High shelf parameters to be tested */
    float iG0, iGInf, iFc;

    for(int ri = 0; ri < nRho; ri++){
        float r = rho[ri];
        for(int ti = 0; ti < nTheta; ti++){
            interpHighShelfParams(theta[ti], r, &iG0, &iGInf, &iFc);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance, iG0, iG0_ref[ri][ti]);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance, iGInf, iGInf_ref[ri][ti]);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance_frq, iFc, iFc_ref[ri][ti]);
        }
    }
}

void test__dvf_calcIIRCoeffs(void){
    /* setup */
    const float acceptedTolerance = 0.00001f; // TODO: revisit these thresholds
    const int   nTheta = 6;
    const float theta[nTheta] = {0.000000,2.300000,47.614000,98.600000,166.200000,180.000000};
    const int   nRho = 5;
    const float rho[nRho] = {1.150000,1.250000,1.570000,2.381000,3.990000};
    const float fs = 44100;
    const float b0_ref[nRho][nTheta] = { /* testIIRCoefs_b0 */
        {8.084117,7.162779,0.733534,0.181211,0.157716,0.157753},
        {5.162983,4.832104,0.847478,0.218379,0.188114,0.188100},
        {2.787888,2.735502,1.052975,0.322169,0.274301,0.274359},
        {1.733188,1.725027,1.162720,0.508950,0.432220,0.432405},
        {1.337147,1.334601,1.133469,0.693396,0.606185,0.606313}
    };
    const float b1_ref[nRho][nTheta] = { /* testIIRCoefs_b1 */
        {-7.521737,-6.689086,-0.525709,-0.092147,-0.075692,-0.072026},
        {-4.880667,-4.570874,-0.654751,-0.113974,-0.090171,-0.086752},
        {-2.654257,-2.604818,-0.917824,-0.171818,-0.130191,-0.126320},
        {-1.659057,-1.651278,-1.090421,-0.278191,-0.201595,-0.195404},
        {-1.283715,-1.281267,-1.082675,-0.387883,-0.278529,-0.268341}
    };
    const float a1_ref[nRho][nTheta] = { /* testIIRCoefs_a1 */
        {-0.958646,-0.960287,-0.849695,-0.833925,-0.769110,-0.755889},
        {-0.965649,-0.965782,-0.866341,-0.818335,-0.743436,-0.731349},
        {-0.966189,-0.966188,-0.910070,-0.775971,-0.682649,-0.670043},
        {-0.965632,-0.965605,-0.948954,-0.713458,-0.602472,-0.587018},
        {-0.964816,-0.964791,-0.959744,-0.661426,-0.540433,-0.522001}
    };
    
    float b0, b1, a1;       /* IIR coeffs to be tested */
    float iG0, iGInf, iFc;

    for(int ri = 0; ri < nRho; ri++){
        float r = rho[ri];
        for(int ti = 0; ti < nTheta; ti++){
            interpHighShelfParams(theta[ti], r, &iG0, &iGInf, &iFc);
            calcIIRCoeffs(iG0, iGInf, iFc, fs, &b0, &b1, &a1);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance, b0, b0_ref[ri][ti]);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance, b1, b1_ref[ri][ti]);
            TEST_ASSERT_FLOAT_WITHIN(acceptedTolerance, a1, a1_ref[ri][ti]);
        }
    }
}
