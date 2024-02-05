import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { Subscription } from 'rxjs/internal/Subscription';
import { HomeService } from 'src/app/bmc-app/modules/nav/home/services';

@Component({
    selector: 'app-onekey-collect',
    templateUrl: './onekey-collect.component.html',
    styleUrls: ['./onekey-collect.component.scss']
})
export class OnekeyCollectComponent implements OnInit {

    public percentage: string = '0%';

    private progressSubscription: Subscription;

    constructor(
        private homeService: HomeService,
        private i18n: TranslateService,

        ) { }

    ngOnInit() {
        const progress$ = this.homeService.getMessage();
        this.progressSubscription = progress$.subscribe((progress) => {
            this.percentage = progress;
        });
    }

    ngOnDestroy(): void {
        this.progressSubscription.unsubscribe();
    }

}
