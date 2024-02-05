import { Pipe, PipeTransform } from '@angular/core';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';

@Pipe({
    name: 'privil'
})
export class PrivilPipe implements PipeTransform {
    constructor(
        private userInfo: UserInfoService
    ) {

    }
    transform(value: string[]): boolean {
        let result: boolean = true;
        if (Array.isArray(value)) {
            result = false;
        }
        result = this.userInfo.hasPrivileges(value);

        return result;
    }

}
