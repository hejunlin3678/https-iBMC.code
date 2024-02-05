import { Component, OnInit } from '@angular/core';
import { UserInfoService } from 'src/app/common-app/service';
import { CertUpdateService } from './services';
import { Router } from '@angular/router';

@Component({
    selector: 'app-cert-update',
    templateUrl: './cert-update.component.html',
    styleUrls: ['./cert-update.component.scss']
})
export class CertUpdateComponent implements OnInit {
    constructor(
        private userService: UserInfoService,
        private certUpdateService: CertUpdateService,
        private router: Router
    ) {}
    public systemLockDownEnabled = this.userService.systemLockDownEnabled;
    public tabs = [
        {
            id: 'ssl',
            route: 'cert-update-ssl',
            title: 'IBMC_SSL_CERTIFICATE_UPDATE',
            active: false,
            hide: false
        },
        {
            id: 'bios',
            route: 'cert-update-bios',
            title: 'IBMC_BIOS_CERTIFICATE_UPDATE',
            active: false,
            hide: true
        }
    ];
    public tabChange(isActive: boolean, id: string) {
        if (isActive) {
            switch (id) {
                case 'ssl':
                    this.router.navigate([
                        '/navigate/user/cert-update/cert-update-ssl'
                    ]);
                    break;
                case 'bios':
                    this.router.navigate([
                        '/navigate/user/cert-update/cert-update-bios'
                    ]);
                    break;
                default:
            }
        }
    }

    ngOnInit(): void {
        this.certUpdateService.getBootOptionsServer().subscribe((res) => {
            const preferredBootMedium = res.body.PreferredBootMedium;
            this.tabs[1].hide = !Object.prototype.hasOwnProperty.call(
                preferredBootMedium,
                'TargetExt'
            );
        });
        const url = this.router.url.split('/');
        const currentRoute = url[url.length - 1];
        this.tabs.forEach((item) => {
            item.active = false;
            if (item.route === currentRoute) {
                item.active = true;
            }
        });
    }
}
