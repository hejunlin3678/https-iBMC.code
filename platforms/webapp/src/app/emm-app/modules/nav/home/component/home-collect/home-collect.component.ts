import { Component, OnInit } from '@angular/core';
import { Subscription } from 'rxjs/internal/Subscription';

import { HomeService } from './../../services/home.service';

@Component({
    selector: 'app-home-emm-collect',
    templateUrl: './home-collect.component.html',
    styleUrls: ['./home-collect.component.scss']
})
export class HomeEmmCollectComponent implements OnInit {

    public percentage: string = '0%';

    private progressSubscription: Subscription;

    constructor(private homeService: HomeService) { }

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
