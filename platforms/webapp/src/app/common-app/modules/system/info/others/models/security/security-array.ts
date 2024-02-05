import { Card } from '../card';
import { Security } from './security';
export class SecurityArray extends Card {

    private securitys: Security[];

    constructor() {
        super();
        this.title = [
            {
                title: 'OTHER_PROTOCOL_TYPE'
            }, {
                title: 'OTHER_PROTOCOL_VERSION'
            }, {
                title: 'COMMON_MANUFACTURER'
            }, {
                title: 'OTHER_VENDOR_VERSION'
            }, {
                title: 'OTHER_SELFCHECK'
            }
        ];
        this.securitys = [];
    }

    get getSecuritys(): Security[] {
        return this.securitys;
    }

    addSecurity(security: Security): void {
        this.securitys.push(security);
    }

}
