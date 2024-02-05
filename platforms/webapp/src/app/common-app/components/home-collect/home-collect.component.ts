import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs/internal/Subscription';

import { CommonService } from '../../service/common.service';

@Component({
    selector: 'app-home-collect',
    templateUrl: './home-collect.component.html',
    styleUrls: ['./home-collect.component.scss']
})
export class HomeCollectComponent implements OnInit, OnDestroy {

    public percentage: string = '0%';

    private progressSubscription: Subscription;

    constructor(private commonService: CommonService) { }

    ngOnInit() {
        const progress$ = this.commonService.getMessage();
        this.progressSubscription = progress$.subscribe((progress) => {
            this.percentage = progress;
        });
    }

    ngOnDestroy(): void {
        this.progressSubscription.unsubscribe();
    }

}
