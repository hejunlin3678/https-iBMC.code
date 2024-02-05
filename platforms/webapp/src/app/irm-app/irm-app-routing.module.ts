import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { IRMAppComponent } from './irm-app.component';
import { KvmLoginGuard, AuthGuard } from '../common-app/guard';
import { IRMGuard } from './guard/irm-guard.service';
import { AppGuard } from '../common-app/guard/app-guard.service';

const routes: Routes = [
    {
		path: '',
        component: IRMAppComponent,
        canActivate: [AppGuard, IRMGuard],
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
    }
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class IrmAppRoutingModule { }
