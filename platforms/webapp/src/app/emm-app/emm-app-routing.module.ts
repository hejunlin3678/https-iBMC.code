import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { AuthGuard, KvmLoginGuard } from '../common-app/guard';
import { EMMAppComponent } from './emm-app.component';
import { EMMGuard } from './guard/emm-guard.service';
import { AppGuard } from '../common-app/guard/app-guard.service';

const routes: Routes = [{
    path: '',
    component: EMMAppComponent,
    canActivate: [AppGuard, EMMGuard],
    children: [
        {
            path: 'login',
            loadChildren: () => import('./modules/login/login.module').then((m) => m.LoginModule),
            canActivateChild: [KvmLoginGuard]
        },
        {
            path: 'navigate',
            loadChildren: () => import('./modules/nav/nav.module').then((m) => m.NavModule),
            canActivateChild: [AuthGuard]
        },
        {
            path: '',
            pathMatch: 'full',
            redirectTo: 'login'
        },
        {
            path: '**',
            pathMatch: 'full',
            redirectTo: 'login'
        }
    ]
}];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})

export class EMMAppRoutingModule {

}
