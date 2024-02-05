
import { TiValidationConfig } from '@cloud/tiny3';

export function getPatternTip(patternTip: string, tip: string): TiValidationConfig {
    return {
        type: 'blur',
        errorMessage: {
            pattern: patternTip
        },
        tipPosition: 'top',
        tip,
        tipMaxWidth: '350px'
    };
}
// 1~48个字符，由数字、英文字母和特殊字符组成，不能包含以下特殊字符：[]。
export const ASSET_SPECIAL_CHAR_REQUIRE = /^[0-9a-zA-Z_ `~!@#$%^&*()-=+{}|;,'.:\"(.*)\"?/<>\\-\\.]{1,48}$/;
export const ASSET_SPECIAL_CHAR = /^[0-9a-zA-Z_ `~!@#$%^&*()-=+{}|;,'.:\"(.*)\"?/<>\\-\\.]{0,48}$/;
// 不能全为空
export const ASSET_SPECIAL_EMPTY = /^(?!(\s+$))/;

