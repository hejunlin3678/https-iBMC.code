import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { Pangen } from 'src/app/pangea-app/models/pangen.model';
import { OceanType } from 'src/app/pangea-app/models/common.datatype';
import { arcticRoutes, atlanticRoutes, atlanticSMMRoutes, pacificRoutes } from './routes';

let routes: Routes;

switch (Pangen.ocean) {
    case OceanType.Arctic:
        routes = arcticRoutes;
        break;
    case OceanType.Atlantic:
        routes = atlanticRoutes;
        break;
    case OceanType.AtlanticSMM:
        routes = atlanticSMMRoutes;
        break;
    case OceanType.Pacific:
        routes = pacificRoutes;
        break;
    default:
        routes = arcticRoutes;
        break;
}

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class MaintanceRoutingModule { }
