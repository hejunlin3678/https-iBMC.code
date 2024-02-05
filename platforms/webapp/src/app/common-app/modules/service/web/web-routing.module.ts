import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { WebGuard } from 'src/app/common-app/guard/web-guard.service';
import { WebComponent } from './web.component';

const routes: Routes = [
    {
        path: '',
        component: WebComponent,
        canActivate: [WebGuard]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class WebRoutingModule { }
