import { Router } from '@angular/router';
import { Component, OnInit, Input } from '@angular/core';
import { IResource } from '../../model/home.datatype';

@Component({
    selector: 'app-irm-home-resource',
    templateUrl: './home-irm-resource.component.html',
    styleUrls: ['./home-irm-resource.component.scss']
})
export class HomeIrmResourceComponent implements OnInit {

    @Input() resource: IResource;

    constructor(private router: Router) { }

    ngOnInit() {
    }

    public clickFn() {
        this.router.navigate(this.resource.targetNav);
    }

}
