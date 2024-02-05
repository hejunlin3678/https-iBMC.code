import { Pipe, PipeTransform } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';

@Pipe({
    name: 'role'
})
export class RolePipe implements PipeTransform {
    constructor(
        private translate: TranslateService
    ) {

    }
    transform(value: string): string {
        let result = '';
        switch (value) {
            case 'Common User':
            case 'Commonuser':
                result = this.translate.instant('COMMON_ROLE_COMMONUSER');
                break;
            case 'Administrator':
                result = this.translate.instant('COMMON_ROLE_ADMINISTRATOR');
                break;
            case 'Operator':
                result = this.translate.instant('COMMON_ROLE_OPERATOR');
                break;
            case 'Custom Role 1':
            case 'CustomRole1':
                result = this.translate.instant('COMMON_ROLE_CUSTOMROLE1');
                break;
            case 'Custom Role 2':
            case 'CustomRole2':
                result = this.translate.instant('COMMON_ROLE_CUSTOMROLE2');
                break;
            case 'Custom Role 3':
            case 'CustomRole3':
                result = this.translate.instant('COMMON_ROLE_CUSTOMROLE3');
                break;
            case 'Custom Role 4':
            case 'CustomRole4':
                result = this.translate.instant('COMMON_ROLE_CUSTOMROLE4');
                break;
            case 'Noaccess':
                result = this.translate.instant('COMMON_ROLE_NOACCESS');
                break;
            default:
                result = '';
        }
        return result;
    }
}
