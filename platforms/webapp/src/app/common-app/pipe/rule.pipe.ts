import { Pipe, PipeTransform } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';

@Pipe({
    name: 'rule'
})
export class RulePipe implements PipeTransform {
    constructor(
        private translate: TranslateService
    ) {

    }
    transform(value: string[]): string {
        let result = [];
        const self = this;
        result = value.map(rule => {
            return self.getRuleText(rule);
        });

        return result.join(' ');
    }

    private getRuleText(rule): string {
        let result = '';
        switch (rule) {
            case 'Rule1':
                result = this.translate.instant('SECURITY_SECUTITY_CFG_RULE1');
                break;
            case 'Rule2':
                result = this.translate.instant('SECURITY_SECUTITY_CFG_RULE2');
                break;
            case 'Rule3':
                result = this.translate.instant('SECURITY_SECUTITY_CFG_RULE3');
                break;
            default:
                result = '';
        }
        return result;
    }
}
