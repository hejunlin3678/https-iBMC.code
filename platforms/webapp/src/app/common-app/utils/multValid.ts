import { AbstractControl, ValidatorFn, ValidationErrors, UntypedFormGroup } from '@angular/forms';
import { MIN_CHARTYPE_2, SPECIAL_CHAR, SPACE_SPECIAL_CHAR, getByte } from './common';
import { TranslateService } from '@ngx-translate/core';

export class MultVaild {
    constructor(
        private translate: TranslateService
    ) {

    }
    // 密码校验必须是包含大写、小写、数字中的2项
    public static pattern(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                pattern: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            if (value === '') {
                return null;
            }
            let count = 0;
            MIN_CHARTYPE_2.forEach((reg) => {
                if (reg.test(value)) {
                    count++;
                }
            });
            if (count > 1) {
                return null;
            } else {
                return errorModel;
            }
        };
    }

    // 至少包含一个空格或以下特殊字符`~!@#$%^&*()-_=+\\|[{}];:\,<.>/?。
    public static specialChartAndSpace(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                specialChartAndSpace: {
                    tiErrorMessage: 'error'
                }
            };
            if (control.value === '') {
                return null;
            }

            if (SPACE_SPECIAL_CHAR.test(control.value)) {
                return null;
            }

            return errorModel;
        };
    }

    // 密码校验不能出现特殊字符SPECIAL_CHAR
    public static special(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                special: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            if (value === '') {
                return null;
            }
            if (SPECIAL_CHAR.test(value)) {
                return null;
            } else {
                return errorModel;
            }
        };
    }
    // 输入不能为空格
    public static noEmpty(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                noEmpty: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            if (value === '') {
                return null;
            }
            if (/[\s]/.test(value)) {
                return errorModel;
            } else {
                return null;
            }
        };
    }

    // 输入不能包括空格
    public static notIncludeSpace(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                notIncludeSpace: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            if (/\s/.test(value)) {
                return errorModel;
            }
            return null;
        };
    }

    // 输入不能全是空白符
    public static notAllWhiteSpace(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                notAllWhiteSpace: {
                    tiErrorMessage: 'error'
                }
            };
            const value = control.value;
            if (/^\s+$/.test(value)) {
                return errorModel;
            }
            return null;
        };
    }

    // 输入字节的长度，不能大于255
    public static maxLength255(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                outOfRange: {
                    tiErrorMessage: 'error'
                }
            };
            const length = getByte(control.value);
            if (length > 255) {
                return errorModel;
            }
            return null;
        };
    }

    // 校验与目标输入值是否一致
    public static equalTo(targetName: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                equalTo: {
                    tiErrorMessage: 'error'
                }
            };
            const target = control.root.get(targetName);
            const value = control.value;
            if (target === null) {
                return null;
            }
            return target.value === value ? null : errorModel;
        };
    }

    // 校验与目标输入值不能相同
    public static notEqualTo(targetName: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const errorModel = {
                notEqualTo: {
                    tiErrorMessage: 'error'
                }
            };
            const target = control.root.get(targetName);
            const value = control.value;
            if (target === null) {
                return null;
            }
            if (value !== '' && (target.value !== '' || target.value !== null)) {
                return target.value !== value ? null : errorModel;
            } else {
                return null;
            }
        };
    }

    // 输入可以为空字符串
    public static emptyCharacter(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            if (control.value === '') {
                return null;
            }
            return null;
        };
    }

    // 组应用文件夹的校验规则
    public static groupFolder(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            if (value === '' || value === undefined || value === null) {
                return null;
            }

            const length = getByte(value);
            if (length > 255) {
                return {
                    outOfRange: {
                        tiErrorMessage: 'error'
                    }
                };
            }

            // 如果任意一项满足下面的正则，则校验失败
            const pattern = /^(CN|OU)=[^,]*$/;
            let result = true;
            const arr = value.split(',');
            result = arr.every((item) => {
                return pattern.test(item);
            });
            if (!result) {
                return {
                    inputError: {
                        tiErrorMessage: 'error'
                    }
                };
            }
            return null;
        };
    }

    // 强制修改密码规则
    public static forceResetPassword(message: string): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            if (value === '') {
                return {
                    forceReset: {
                        tiErrorMessage: message
                    }
                };
            }
            return null;
        };
    }

    // 用户名不能为.或..
    public static notPoints(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            if (value === '.' || value === '..') {
                return {
                    notPoints: {
                        tiErrorMessage: 'error'
                    }
                };
            }
            return null;
        };
    }

    // 邮件地址校验
    public static emailValidator(): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            if (!value) {
                return null;
            }
            const errorModel = {
                emailValidator: {
                    tiErrorMessage: 'error'
                }
            };

            // 不能包含空格
            if (value.indexOf(' ') > -1) {
                return errorModel;
            }

            let iAtPos = 0;
            let lastPos = 0;
            let iAtTimes = 0;
            let i = 0;
            for (const char of value) {
                if (char === '@') {
                    iAtPos = i;
                    iAtTimes++;
                } else if (char === '.') {
                    lastPos = i;
                }
                i++;
            }

            /**
             * 不能大于255个字符
             * '@'不能是第一个字符
             * '@'和最后一个'.'之间不能少于1个字符
             * 最后一个'.'之后必须还有至少2个字符
             * 最后一个'.'之后的字符个数不能大于63个
             * '@'只能有一个
             */
            if (i > 255 || iAtPos < 1 || (lastPos - 2) < iAtPos || (i - lastPos) < 3 || (i - lastPos) > 64 || iAtTimes !== 1) {
                return errorModel;
            }
            return null;
        };
    }

    // 校验IP是否在冲突列表中
    public static conflictIp(formArr: { formGroup: UntypedFormGroup, ipControlName: string }[], ipName, conFlicts: string[]): ValidatorFn {
        return (control: AbstractControl): ValidationErrors | null => {
            const value = control.value;
            if (formArr.length === 0 || conFlicts.length === 0 || value === '' || value === null) {
                return null;
            }
            const error = {
                conflict: {
                    tiErrorMessage: ''
                }
            };

            const result = formArr.filter(form => {
                if (form.ipControlName === ipName) {
                    return false;
                }
                return form.formGroup.value[form.ipControlName] === value;
            });

            // 在IP列表种存在或与主背板静态IP及浮动IP相同者，视为IP冲突
            if (result.length > 0 || conFlicts.indexOf(value) > -1) {
                return error;
            }
            return null;
        };
    }
}
