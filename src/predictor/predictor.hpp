//
// Created by Hyunseung Ryu on 2025. 6. 25..
//

#ifndef PREDICTOR_HPP
#define PREDICTOR_HPP

/**
 * TODO: Predictor Input
 *  - 1번 카메라로부터 x, theta를 구해야 함
 *      - 1번 카메라가 천장이면 좋겠다
 *  - 2번 카메라로부터 y, z를 구해야 함
 *      - 사영을 역으로 하면 됨
 *
 * TODO: Predictor Impl
 *  1. 2번 카메라가 처음 탁구공 위치 및 시각, 처음 최고점을 찍었을 때의 위치 및 시각, 처음 바닥에 튕겼을 때의 위치 및 시각을 얻음
 *  2. 얻은 위치를 역사영하여 찌그러진 포물선을 정상적인 포물선화함
 *  3. quadratic fit 하여 이후 궤적을 예측, 반대쪽 탁구대 끝에 언제, 어디에 도달할 지 예측
 *  4. 휴리스틱하게 구한 값으로 탁구공 치기
 */

#endif //PREDICTOR_HPP
