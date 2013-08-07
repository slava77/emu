#include "csc_display/graph.h"

void addHeader(TVirtualPad *c, int event_num, int chamberID, char* configStr) {
    char eventBuf[30];
    char chamberBuf[30];
    
    sprintf(eventBuf, "Event #%d", event_num);
    sprintf(chamberBuf, "Chamber #%d", chamberID);
    
    // change to this pad
    c->cd();
    
    TText* text = new TText;
    text->SetTextSize(0.5);
    text->SetTextFont(82);
    text->SetTextAlign(23);
    text->SetTextColor(1);
    text->DrawText(0.5, 0.7, configStr);
    
    text->SetTextSize(0.35);
    text->DrawText(0.3, 0.2, eventBuf);
    text->DrawText(0.7, 0.2, chamberBuf);
}

void getWiresAndStripsGraph (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size, bool peak) {
    // draw on the canvas
    c->cd();
    
    // draw pads
    TPad* stripsPad = new TPad("strips", "strips", 0.05, 0.3, 1, 1);
    TPad* stripsVLabelPad = new TPad("stripsY", "stripsY", 0, 0.3, 0.05, 1);
    TPad* wiresPad = new TPad("wires", "wires", 0, 0, 1, 0.3);
    stripsPad->Draw();
    stripsVLabelPad->Draw();
    wiresPad->Draw();
    
    // divide it so that layer 2 -> NLAYER+1 are for strips, 0 for ME1/1 label and NLAYER+2 for Strip number label
    stripsPad->Divide(1, NLAYER+2, 0, 0);
    
    // allocate histograms
    TH1F** hist;
    hist = (TH1F**) malloc(sizeof(TH1F*) * NLAYER);
    
    // plot all important graphics
    wiresStripsLabel(stripsPad);
    verticalWiresStripsLabel(stripsVLabelPad);
    plotWires(wiresPad, data, j_data);
    plotStrips(stripsPad, data, j_data, hist, peak);
    
    getCanvasBuffer(c->GetMother(), imgBuf, size);
    
    // removing all memory leak problems
    for (int i = 0; i < NLAYER; i++) delete hist[i];
    free(hist);
}

void getCLCTTimeGraph (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size) {
    int clct_time_tmp;
    float x_coord[2];
    float y_coord[2];
    float x_shift, x_step, y_shift, y_step, y_time_step;
    int color, i, j, ij;
    int text_index[NLAYER][NSTRIP/2];
    char t[NLAYER][NSTRIP/2];
    char buffer[30];
    
    TLine* line = new TLine;
    TText* text = new TText;
    TText* text_draw = new TText;

    x_shift = 0.01;
    x_step = 0.85/(NSTRIP/2);
    y_shift = 0.85;
    y_step = 0.6/(NLAYER*data.clct_nbucket);
    y_time_step = (NLAYER+1)*y_step;

    c->cd();
    c->Clear();
    line->SetLineWidth(6);
    text->SetTextSize(0.040);
    text->SetTextFont(102);

    for (j=1; j<=NLAYER; j++) {
        for (i=1; i<=NSTRIP/2; i++) {
            text_index[j-1][i-1] = 0;
        }
    }

    color = 1;
    for (i=1; i<=data.clct_nbucket; i++) {
        if (color==1) {
            line->SetLineColor(4);
            color = 0;
        }
        else {
            line->SetLineColor(3);
            color = 1;
        }
        sprintf(buffer, "%d", i);
        for (j=1; j<=NLAYER; j++) {
            x_coord[0] = x_shift + x_step;
            x_coord[1] = x_shift + (NSTRIP/2+1)*x_step;
            y_coord[0] = y_shift - i*y_time_step - j*y_step;
            y_coord[1] = y_coord[0];
            line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
            if (j==(NLAYER/2+2)) {
                text->SetTextSize(0.04);
                text->DrawText(x_coord[1]+0.01, y_coord[0], buffer);
            }
            if ((j==NLAYER&&i==data.clct_nbucket)) {
                text->SetTextAlign(22);
                text->SetTextSize(0.04);
                sprintf(buffer, "%d", 1);
                text->DrawText(x_coord[0]+0.005, y_coord[0]-0.03, buffer);
                sprintf(buffer, "%d", NSTRIP/2);
                text->DrawText(x_coord[1]-0.005, y_coord[0]-0.03, buffer);
                text->DrawText((x_coord[1]-x_coord[0])/2, y_coord[0]-0.05, "Distrip Number");
            }
        }
    }

    line->SetLineColor(2);
    std::cout<<"clct_nbucket: "<<data.clct_nbucket<<endl;
    for (i=1; i<=data.clct_nbucket; i++) {
        for (j=1; j<=NLAYER; j++) {
            for (ij=1; ij<=(NSTRIP); ij++) {
                clct_time_tmp = data.clct_dump_strips[j-1][ij-1];
                if ((1&clct_time_tmp>>(i-1))==1) {
                    x_coord[0] = x_shift + ij/2*x_step;
                    x_coord[1] = x_shift + (ij/2+1)*x_step;
                    y_coord[0] = y_shift - i*y_time_step - j*y_step;
                    y_coord[1] = y_coord[0];
                    line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
                    text_index[j-1][ij/2-1]++;
                }
            }
        }
    }

    for (j=1; j<=NLAYER; j++) {
        for (i=1; i<=NSTRIP/2; i++) {
            t[j-1][i-1] = '*';
            switch (text_index[j-1][i-1]) {
                case 1:
                    t[j-1][i-1] = '1';
                    break;
                case 2:
                    t[j-1][i-1] = '2';
                    break;
                case 3:
                    t[j-1][i-1] = '3';
                    break;
                case 4:
                    t[j-1][i-1] = '4';
                    break;
                case 5:
                    t[j-1][i-1] = '5';
                    break;
                case 6:
                    t[j-1][i-1] = '6';
                    break;
                case 7:
                    t[j-1][i-1] = '7';
                    break;
                case 8:
                    t[j-1][i-1] = '8';
                    break;
                case 9:
                    t[j-1][i-1] = '9';
                    break;
                default:
                    t[j-1][i-1] = '*';
            }
        }
    }

    double dy = .018;
    double dx, x0;
    x0 = .1;
    dx = .022;
    text_draw->SetTextFont(82);
    text_draw->SetTextSize(.031);
    for (i=1; i<=NLAYER; i++) {
        for (j=1; j<=NSTRIP/2; j++) {
            sprintf(buffer, "%c", t[i-1][j-1]);
            text_draw->DrawText(x_shift + j*x_step, .95-i*dy, buffer);
        }
    }

    c->Update();
    
    getCanvasBuffer(c->GetMother(), imgBuf, size);
    
    delete text_draw;
    delete text;
    delete line;
}

void getALCTTimeGraph (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size) {
    char buffer[30];
    c->cd();
    
    int alct_time_tmp;
    float x_coord[2], y_coord[2];
    float x_shift, x_step, y_shift, y_step, y_time_step;
    int color, i, j, ij;

    TLine* line = new TLine;
    TText* text = new TText;

    x_shift = 0.01;
    x_step = 0.85/(NWIRE);
    y_shift = 0.98;
    y_step = 0.7/(NLAYER*data.alct_nbucket);
    y_time_step = (NLAYER+1)*y_step;
    
    c->Clear();
    line->SetLineWidth(5); // was: 4
    text->SetTextSize(0.040);
    text->SetTextFont(102);
    
    line->SetLineColor(1);
    for (j=1; j<=NLAYER; j++) {
        x_coord[0] = x_shift + x_step;
        x_coord[1] = x_shift + (NWIRE+1)*x_step;
        y_coord[0] = y_shift - 0*y_time_step - j*y_step;
        y_coord[1] = y_coord[0];
        line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
        if (j==(NLAYER/2+2)) {
            sprintf(buffer, "OR");
            text->DrawText(x_coord[1]+.01, y_coord[0], buffer);
        }
    }

    line->SetLineColor(2);
    for (j=1; j<=NLAYER; j++) {
        for (ij=1; ij<=NWIRE; ij++) {
            alct_time_tmp = data.alct_dump[j-1][ij-1];
            if (alct_time_tmp != 0) {
                x_coord[0] = x_shift + ij*x_step;
                x_coord[1] = x_shift + (ij+1)*x_step;
                y_coord[0] = y_shift - 0*y_time_step - j*y_step;
                y_coord[1] = y_coord[0];
                line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
            }
        }
    }

    color = 1;
    for (i=1; i<=data.alct_nbucket; i++) {
        if (color==1) {
            line->SetLineColor(4);
            color = 0;
        }
        else {
            line->SetLineColor(3);
            color = 1;
        }
        sprintf(buffer, "%d", i);
        for (j=1; j<=NLAYER; j++) {
            x_coord[0] = x_shift + x_step;
            x_coord[1] = x_shift + (NWIRE+1)*x_step;
            y_coord[0] = y_shift - i*y_time_step - j*y_step;
            y_coord[1] = y_coord[0];
            line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
            if (j==(NLAYER/2+2)) {
                text->SetTextSize(0.04);
                text->DrawText(x_coord[1]+0.01, y_coord[0], buffer);
            }
            if ((j==NLAYER&&i==data.alct_nbucket)) {
                text->SetTextAlign(22);
                text->SetTextSize(0.04);
                sprintf(buffer, "%d", 1);
                text->DrawText(x_coord[0]+0.005, y_coord[0]-0.03, buffer);
                sprintf(buffer, "%d", NWIRE);
                text->DrawText(x_coord[1]-0.005, y_coord[0]-0.03, buffer);
                text->DrawText((x_coord[1]-x_coord[0])/2, y_coord[0]-0.05, "Wire Group Number");
            }
        }
    }
    line->SetLineColor(2);
    for (i=1; i<=data.alct_nbucket; i++) {
        for (j=1; j<=NLAYER; j++) {
            for (ij=1; ij<=NWIRE; ij++) {
                alct_time_tmp = data.alct_dump[j-1][ij-1];
                if ((1&(alct_time_tmp)>>(i-1))==1) {
                    x_coord[0] = x_shift + ij*x_step;
                    x_coord[1] = x_shift + (ij+1)*x_step;
                    y_coord[0] = y_shift - i*y_time_step - j*y_step;
                    y_coord[1] = y_coord[0];
                    line->DrawLine(x_coord[0], y_coord[0], x_coord[1], y_coord[1]);
                }
            }
        }
    }
    c->Update();
    
    getCanvasBuffer(c->GetMother(), imgBuf, size);
    
    delete line;
    delete text;
    
    return;
}

void getCFEBTimeScaleGraph (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size) {
    int i, j;
    
    // allocate graph object for cfeb time scale plot
    TGraph*** graph;
    graph = (TGraph***) malloc(sizeof(TGraph**)*NLAYER);
    for (i = 0; i < NLAYER; i++) {
        graph[i] = (TGraph**) malloc(sizeof(TGraph*)*5);
    }
    
    // divide the canvas into 2 parts: left and right
    float hBorder = 0.15;
    TPad* lPad = new TPad("left", "left", 0, 0, hBorder, 1);
    TPad* rPad = new TPad("right", "right", hBorder, 0, 1, 1);
    c->cd();
    lPad->Draw();
    rPad->Draw();
    
    // print the keys on the left pad
    drawCFEBTimeScaleKeys(lPad);
    
    // plot the graph on the right pad
    rPad->Divide(1, NLAYER+1, 0, 0);
    plotCFEBTimeScale(rPad, data, j_data, graph);
    
    // add horizontal axis on the right pad
    drawCFEBTimeScaleHAxis(rPad);
    
    // get image of all what on canvas
    getCanvasBuffer(c->GetMother(), imgBuf, size);
    
    // deallocate the graph object
    for (i = 0; i < NLAYER; i++) {
        for (j = 0; j < 5; j++) delete graph[i][j];
        free(graph[i]);
    }
}

void getCtrigGraph (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size) {
    // allocate ctrighist
    TH1F** ctrighist;
    ctrighist = (TH1F**) malloc(sizeof(TH1F*) * NLAYER);
    
    // divide the canvas into 2 parts: left and right
    float hBorder = 0.15;
    TPad* lPad = new TPad("left", "left", 0, 0, hBorder, 1);
    TPad* rPad = new TPad("right", "right", hBorder, 0, 1, 1);
    c->cd();
    lPad->Draw();
    rPad->Draw();
    
    // plot the histogram
    rPad->Divide(1, NLAYER*2+2, 0, 0);
    plotCtrigStrips(rPad, data, j_data, ctrighist);
    
    // plot the keys and axis
    drawCtrigKeys(lPad);
    drawCtrigAxis(rPad);
    
    // get image of all what on canvas
    getCanvasBuffer(c->GetMother(), imgBuf, size);
    
    // deallocate ctrighist
    for (int i = 0; i < NLAYER; i++) delete ctrighist[i];
    free(ctrighist);
}

void getAtrigGraph (TVirtualPad* c, data_type data, j_common_type j_data, char** imgBuf, int* size) {
    // preprocessing
    atrigWireGeomCc(&j_data);
    
    // plot the atrig wires
    plotAtrigWires(c, data, j_data);
    
    // get image of all what on canvas
    getCanvasBuffer(c->GetMother(), imgBuf, size);
}

/************************** Other Supporting Graphics Functions **************************/
/******** for Wires And Strips ********/
void wiresStripsLabel(TVirtualPad* c) {

    TText* text = new TText;
    TGaxis* axis = new TGaxis;
    TLine* line = new TLine;

    // c = hmidcan->GetCanvas();
    c->cd(NLAYER+2);
    text->SetTextAngle(0);

    gPad->SetPad(gPad->GetXlowNDC(),0,gPad->GetXlowNDC()+gPad->GetWNDC(),1); //expand height
    gPad->SetFillStyle(4001); // make transparent
    text->SetTextSize(0.05);
    text->SetTextFont(82);
    text->SetTextAlign(23);
    text->SetTextColor(1);
    text->DrawText(0.5, 0.07, "Strip Number");

    float histstop = 0.87;
    float histsbottom = 0.135;
    line->SetLineColor(1);
    line->SetLineWidth(1);
    line->SetLineStyle(1);
    line->DrawLine(striphist_x1, histsbottom, striphist_x1, histstop);
    line->DrawLine(striphist_x2, histsbottom, striphist_x2, histstop);

    // x-axis at the bottom
    axis->SetLabelSize(0.04);
    axis->SetLabelColor(1);
    axis->DrawAxis(striphist_x1, histsbottom, striphist_x2, histsbottom, 0, NSTRIP+2, 3206);

    line->SetLineColor(17);
    line->SetLineStyle(3);
    for(int i=1; i<NCFEB; ++i){
        double dx = (striphist_x2-striphist_x1)/(NSTRIP+2);
        double cfebedge = striphist_x1 + dx + dx*i*NCFEB_STRIP;
        line->DrawLine(cfebedge, histsbottom, cfebedge, histstop);
    }
    if(NCFEB==7){
        int i=4;
        double dx = (striphist_x2-striphist_x1)/(NSTRIP+2);
        double cfebedge = striphist_x1 + dx + dx*i*NCFEB_STRIP;
        line->SetLineColor(17);
        line->SetLineWidth(2);
        line->SetLineStyle(1);
        line->DrawLine(cfebedge, histsbottom-0.02, cfebedge, histstop+0.03);
        c->cd(1);
        text->SetTextSize(0.4);
        text->SetTextFont(82);
        text->SetTextAlign(23);
        text->SetTextColor(1);
        text->DrawText(0.35, 0.4, "ME1/1B");
        text->DrawText(0.80, 0.4, "ME1/1A");
    }
    
    c->Update();
    
    delete text;
    delete axis;
    delete line;
}

void verticalWiresStripsLabel(TVirtualPad* c) {
    c->cd();
    TText* text = new TText;
    
    float histstop = 0.87;
    float histsbottom = 0.135;
    
    text->SetTextAlign(21);
    text->SetTextFont(82);
    text->SetTextSize(0.4);
    text->DrawText(1, histstop - 0.05, "Layer 1");
    text->DrawText(1, histsbottom + 0.05, "Layer 6");
    
    text->SetTextAngle(90);
    text->SetTextSize(0.6);
    text->SetTextAlign(23);
    text->DrawText(1, 0.50, "ADC Counts");
    c->Update();
}

void plotWires(TVirtualPad* c, data_type data, j_common_type j_data) {
    int i, j, ilayer, igroup;
    double x0, y0, dx, dy, x1, x2, y1, y2;
    int wghv32[4] = { 12, 22, 22, 22 };
    int wghv64[4] = { 16, 28, 40, 52 };
    int wghv96[4] = { 32, 64, 64, 64 };
    int wghv112[4] = { 44, 80, 80, 80 };
    char buffer[20];
    
    TLine* line = new TLine;
    TText* text = new TText;
    TBox* box = new TBox;
    
    c->cd();
    c->Clear();

    box->SetFillStyle(0);
    box->SetLineColor(4);
    line->SetLineColor(4);

    text->SetTextAlign(23);
    text->SetTextSize(0.09);
    text->SetTextFont(102);
    text->DrawText(0.5, 0.95, "Wires");

    text->SetTextAlign(21);
    text->SetTextSize(.085);
    text->DrawText(0.5, 0.1, "Wire Group Number");

    x0 = 0.10;
    y0 = 0.2;
    dx = 0.80/(NWIRE);
    dy = 0.60/NLAYER;

    x1 = x0;
    y1 = y0;
    x2 = x1 + dx;
    y2 = y1 + dy;

    line->DrawLine(x1, y0, x1, y0 + NLAYER*(dy+.007)-.007);

    for (i=1; i<=NLAYER; i++) {
        for (j=1; j<=NWIRE; j++) {
            box->DrawBox(x1, y1, x2, y2);
            x1+=dx;
            x2+=dx;
            if (i==1&&(j%8==0)) {
                line->DrawLine(x1, y0, x1, y0 + NLAYER*(dy+.007)-.007);
            }
        }
        x1 = x0;
        x2 = x1+dx;
        y1 = y1 + dy + .007;
        y2 = y1 + dy;
    }

    line->SetLineColor(7);
    y1 = y0;
    y2 = y0 + NLAYER*(dy+.007) - .007;
    if (NWIRE==48) {}
    else {
        for (j=0; j<4; j++) {
            if (NWIRE==32) {
                x1 = x0 + wghv32[j]*dx;
            }
            else if (NWIRE==64) {
                x1 = x0 + wghv64[j]*dx;
            }
            else if (NWIRE==96) {
                x1 = x0 + wghv96[j]*dx;
            }
            else if (NWIRE==112) {
                x1 = x0 + wghv112[j]*dx;
            }
            x2 = x1;
            line->DrawLine(x1, y1, x2, y2);
        }
    }

    x1 = x0;
    y1 = y0 + NLAYER*(dy+.007) - .007;
    x2 = x1 + dx;
    y2 = y1 - dy;

    box->SetFillStyle(1001);
    box->SetFillColor(4);
    text->SetTextAlign(33);
    text->SetTextSize(.05);
    text->SetTextFont(82);
    
    for (ilayer=1; ilayer<=6; ilayer++) {
        if (ilayer==1||ilayer==6) {
            sprintf(buffer, "Layer %d", ilayer);
            text->SetTextSize(0.08);
            text->DrawText(x1-.01, y1-.02, buffer);
        }
        
        for (igroup=1; igroup<=NWIRE; igroup++) {
            if ((ilayer==6)&&(igroup==1||igroup==NWIRE)) {
                text->SetTextSize(0.08);
                text->SetTextAlign(13);
                sprintf(buffer, "%d", igroup);
                text->DrawText(x1, y2-.02, buffer);
            }
            if ((bool)j_data.wiregroup[igroup-1][ilayer-1]) {
                box->DrawBox(x1, y1, x2, y2);
            }
            x1+=dx;
            x2+=dx;
        }
        
        x1 = x0;
        x2 = x1+dx;
        y1 = y1 - dy - .007;
        y2 = y1 - dy;
    }
    
    delete line;
    delete text;
    delete box;
}

void plotStrips(TVirtualPad* c, data_type data, j_common_type j_data, TH1F** hist, bool peak) {
    char buffer[10];
    int layer_no, i;
    
    scale_y(&j_data, peak);
    gStyle->SetOptStat(0);
    gStyle->SetLabelSize(0, "X");
    gStyle->SetLabelSize(.1, "Y");
    
    // create histogram objects
    for (i = 0; i < NLAYER; i++) {
        sprintf(buffer, "hist%d", i);
        hist[i] = new TH1F(buffer, "", NSTRIP+2, 0, NSTRIP+1);
    }
    
    // fill in the histogram
    for (layer_no = 0; layer_no < NLAYER; layer_no++) {
        hist[layer_no]->SetFillStyle(1001);
        hist[layer_no]->SetFillColor(2);
        for (i = 0; i < NSTRIP; i++) {
            float value;
            if (peak)   value = j_data.strips_peaks[i][layer_no];
            else        value = j_data.strips[i][layer_no];
            
            if (value < 0) value *= 0.1;
            hist[layer_no]->Fill((double)i+1, value);
        }
    }

    for (layer_no = 0; layer_no < NLAYER; layer_no++) {
        sprintf(buffer, "Layer %d", layer_no+1); // set y-axis title
        (hist[layer_no]->GetYaxis())->SetRangeUser(j_data.ymin[layer_no], j_data.ymax[layer_no]);
        (hist[layer_no]->GetYaxis())->SetNdivisions(404);
        (hist[layer_no]->GetYaxis())->SetLabelSize(0.2);
        (hist[layer_no]->GetYaxis())->SetTitle("asdf");
        (hist[layer_no]->GetYaxis())->RotateTitle();
        c->cd(layer_no+2);
        gPad->Clear();
        hist[layer_no]->Draw();
        gPad->Update();
    }
    
}

/******** for CFEB Time Samples ********/
void plotCFEBTimeScale(TVirtualPad* c, data_type data, j_common_type j_data, TGraph*** graph) {
    
    int layer_no, istrip, isample;
    float px[j_data.nsamples], py[j_data.nsamples];
    int lineStyle[5] = {9, 1, 2, 4, 3};
    int lineColor[5] = {2, 3, 1, 6, 4};
    char buffer[30];
    
    TText* text = new TText;
    
    scale_cfeb_time_sample_y(&j_data);
    
    gStyle->SetOptStat(0);
    gStyle->SetLabelSize(.12, "X");
    gStyle->SetLabelSize(.1, "Y");
    
    //create graph
    if (!j_data.nsamples) return;
    for (layer_no=1; layer_no<=NLAYER; layer_no++) {
        for (istrip=1; istrip<=5; istrip++) {
            for (isample=1; isample<=j_data.nsamples; isample++) {
                px[isample-1] = isample;
                py[isample-1] = j_data.sample[istrip-1][layer_no-1][isample-1];
            }
            // if(graph[layer_no-1][istrip-1]) delete graph[layer_no-1][istrip-1];
            graph[layer_no-1][istrip-1] = new TGraph(j_data.nsamples, px, py);
        }
    }
    
    //draw graphs
    text->SetTextFont(82);
    text->SetTextSize(.13);
    text->SetTextAlign(11);
    for (layer_no=1; layer_no<=NLAYER; layer_no++) {
        c->cd(layer_no);
        for (istrip=1; istrip<=5; istrip++) {
            sprintf(buffer, "CP");
            if (istrip==1) {
                sprintf(buffer, "ACP");
                (graph[layer_no-1][0]->GetYaxis())->SetRangeUser(j_data.ymin[layer_no-1], j_data.ymax[layer_no-1]);
            }
            graph[layer_no-1][istrip-1]->SetMarkerStyle(7);
            graph[layer_no-1][istrip-1]->SetMarkerColor(lineColor[istrip-1]);
            graph[layer_no-1][istrip-1]->SetLineStyle(lineStyle[istrip-1]);
            graph[layer_no-1][istrip-1]->SetLineColor(lineColor[istrip-1]);
            graph[layer_no-1][istrip-1]->Draw(buffer);
            c->Update();
        }
        sprintf(buffer, "  Peak Strip: %d", j_data.peak_strip[layer_no-1]);
        text->DrawText(.35, .6, buffer);
        c->Update();
    }
    
    delete text;
}

void drawCFEBTimeScaleKeys(TVirtualPad* c) {
    float keytop=0.50;
    float keyheight = 0.22;
    float y;
    
    TText* text = new TText;
    TLine* line = new TLine;
    
    c->cd();
    
    //create key
    text->SetTextSize(0.2);
    text->DrawText(0.4, keytop-keyheight*1/9, "Key");
    text->SetTextFont(82);
    text->SetTextAlign(23);
    
    line->SetLineStyle(1);
    line->SetLineColor(1);
    y=keytop-keyheight*1/7;
    line->DrawLine(0.25, y, 0.85, y);
    text->SetTextAlign(12);
    text->SetTextSize(0.13);
    text->SetTextColor(2);
    y=keytop-keyheight*2/7;
    text->DrawText(0.05, y, "Peak-2");
    line->SetLineColor(2);
    line->SetLineStyle(9);
    line->DrawLine(0.7, y, 0.97, y);
    text->SetTextColor(3);
    y=keytop-keyheight*3/7;
    text->DrawText(0.05, y, "Peak-1");
    line->SetLineColor(3);
    line->SetLineStyle(1);
    line->DrawLine(0.7, y, 0.97, y);
    text->SetTextColor(1);
    y=keytop-keyheight*4/7;
    text->DrawText(0.05, y, "Peak Strip");
    line->SetLineColor(1);
    line->SetLineStyle(2);
    line->DrawLine(0.8, y, 0.97, y);
    text->SetTextColor(6);
    y=keytop-keyheight*5/7;
    text->DrawText(0.05, y, "Peak+1");
    line->SetLineColor(6);
    line->SetLineStyle(4);
    line->DrawLine(0.7, y, 0.97, y);
    text->SetTextColor(4);
    y=keytop-keyheight*6/7;
    text->DrawText(0.05, y, "Peak+2");
    line->SetLineColor(4);
    line->SetLineStyle(3);
    line->DrawLine(0.7, y, 0.97, y);
    c->Update();

    text->SetTextColor(1);
    text->SetTextAlign(22);
    text->SetTextSize(0.2);
    text->DrawText(0.5, 0.91, "Layer 6");
    text->DrawText(0.5, 0.21, "Layer 1");
    text->SetTextAngle(90);
    text->DrawText(0.8, 0.7, "ADC Counts");
    text->SetTextAngle(0);
    c->Update();
    
    delete text;
    delete line;
}

void drawCFEBTimeScaleHAxis(TVirtualPad* c) {
    TText* text = new TText;
    
    // expand canvas for bottom layer to show x-axis
    c->cd(NLAYER);
    gPad->SetPad(gPad->GetXlowNDC(),gPad->GetYlowNDC()-gPad->GetHNDC()*0.1,gPad->GetXlowNDC()+gPad->GetWNDC(),gPad->GetYlowNDC()+gPad->GetHNDC());
    gPad->SetBottomMargin(0.1);

    // x-axis label
    c->cd(NLAYER+1);
    gPad->SetPad(0,0,1,0.1);
    text->SetTextAngle(0);
    text->SetTextSize(0.3);
    text->SetTextAlign(23);
    text->DrawText(0.47, 0.94, "CFEB Data Time Sample");
    c->Update();
    
    delete text;
}

void scale_cfeb_time_sample_y(j_common_type* j_data) {
    int isample, layer_no, trace;

    for (layer_no=1; layer_no<=NLAYER; layer_no++) {
        (*j_data).ymin[layer_no-1] = 0;
        (*j_data).ymax[layer_no-1] = 50.0;
        for (trace=1; trace<=5; trace++) {
            for (isample=1; isample <= (*j_data).nsamples; isample++) {
                float value = (*j_data).sample[trace-1][layer_no-1][isample-1];
                if (value > (*j_data).ymax[layer_no-1]) {
                    (*j_data).ymax[layer_no-1] = value;
                }
                if (value < (*j_data).ymin[layer_no-1]) {
                    (*j_data).ymin[layer_no-1] = value;
                }
            }
        }
        
        float dScale = .1*((*j_data).ymax[layer_no-1] - (*j_data).ymin[layer_no-1]);
        (*j_data).ymax[layer_no-1] += dScale;
        (*j_data).ymin[layer_no-1] -= dScale;
    }
}

/******** for Ctrig ********/
void plotCtrigStrips(TVirtualPad* c, data_type data, j_common_type j_data, TH1F** ctrighist) {
    int i, layer_no, j;
    double x0, x1, dx;
    char buffer[30];
    bool softhalfstrip[2][NSTRIP][NLAYER];
    
    getSWHalfStrip(j_data, softhalfstrip);
    
    scale_ctrig_y(&j_data);

    gStyle->SetOptStat(0);
    gStyle->SetLabelSize(.25, "X");
    gStyle->SetLabelSize(.2, "Y");
    gStyle->SetNdivisions(505, "Y");

    for (i=1; i<=NLAYER; i++) {
        sprintf(buffer, "ctrighist%d", i-1);
        ctrighist[i-1] = new TH1F(buffer, "", NSTRIP+2, 0, NSTRIP+1);
    }

    //fill histograms
    for (layer_no=1; layer_no<=NLAYER; layer_no++) {
        ctrighist[layer_no-1]->SetFillStyle(1001);
        ctrighist[layer_no-1]->SetFillColor(2);
        for (i=1; i<=NSTRIP; i++) {
            if (j_data.strips[i-1][layer_no-1]<0) {
                j_data.strips[i-1][layer_no-1] *= .1;
            }
            ctrighist[layer_no-1]->Fill((double)i, j_data.strips[i-1][layer_no-1]);
        }
    }

    dx = (striphist_x2 - striphist_x1)/(ctrighist[0]->GetNbinsX());
    x0 = striphist_x1;

    for (i=1; i<=2*NLAYER; i++) {
        c->cd(i+1);

        if (i%2==1) { // Draw CFEB data
            int index = (i-1)/2;
            (ctrighist[index]->GetYaxis())->SetRangeUser(j_data.ymin[index], j_data.ymax[index]);
            ctrighist[index]->Draw();
        }
        else { // Draw trigger information
            TBox* box = new TBox;
            float btop = 1.0;
            float bsize = 0.66;
            for (j=1; j<=NSTRIP; j++) {
                x1 = x0 + j*dx;

                box->SetLineColor(1);

                // Software halfstrip from CFEB data
                if (softhalfstrip[0][j-1][(i/2)-1]) {
                    box->SetFillStyle(1001);
                    box->SetFillColor(4);
                    box->DrawBox(x1, btop-bsize*0/3, x1+dx/2.0, btop - bsize*1/3);
                    box->SetFillStyle(0);
                    box->SetFillColor(1);
                    box->DrawBox(x1, btop-bsize, x1+dx/2.0, btop);
                }
                if (softhalfstrip[1][j-1][(i/2)-1]) {
                    box->SetFillStyle(1001);
                    box->SetFillColor(4);
                    box->DrawBox(x1+dx/2.0, btop-bsize*0/3, x1+dx, btop - bsize*1/3);
                    box->SetFillStyle(0);
                    box->SetFillColor(1);
                    box->DrawBox(x1+dx/2.0, btop-bsize, x1+dx, btop);
                }

                // Comparator halfstrips
                if (j_data.halfstrip[0][j-1][(i/2)-1]) {
                    box->SetFillStyle(1001);
                    box->SetFillColor(6);
                    box->DrawBox(x1, btop-bsize*1/3, x1+dx/2.0, btop - bsize*2/3);
                    box->SetFillStyle(0);
                    box->SetFillColor(1);
                    box->DrawBox(x1, btop-bsize, x1+dx/2.0, btop);
                }
                if (j_data.halfstrip[1][j-1][(i/2)-1]) {
                    box->SetFillStyle(1001);
                    box->SetFillColor(6);
                    box->DrawBox(x1+dx/2.0, btop-bsize*1/3, x1+dx, btop - bsize*2/3);
                    box->SetFillStyle(0);
                    box->SetFillColor(1);
                    box->DrawBox(x1+dx/2.0, btop-bsize, x1+dx, btop);
                }

                // CLCT key halfstrip
                if (j_data.clct[0][j-1][(i/2)-1]) {
                    box->SetFillStyle(1001);
                    box->SetFillColor(3);
                    box->DrawBox(x1, btop-bsize*2/3, x1+dx/2.0, btop - bsize*3/3);
                    box->SetFillStyle(0);
                    box->SetFillColor(1);
                    box->DrawBox(x1, btop-bsize, x1+dx/2.0, btop);
                }
                if (j_data.clct[1][j-1][(i/2)-1]) {
                    box->SetFillStyle(1001);
                    box->SetFillColor(3);
                    box->DrawBox(x1+dx/2.0, btop-bsize*2/3, x1+dx, btop - bsize*3/3);
                    box->SetFillStyle(0);
                    box->SetFillColor(1);
                    box->DrawBox(x1+dx/2.0, btop-bsize, x1+dx, btop);
                }
            }
            delete box;
        }
        gPad->Update();
    }

}

void getSWHalfStrip(j_common_type j_data, bool softhalfstrip[2][NSTRIP][NLAYER]) {
    int ilayer, istrip;
    const int comparator_threshold = 50;

    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        for (istrip=1; istrip<=NSTRIP; istrip++) {
            softhalfstrip[0][istrip-1][ilayer-1] = false;
            softhalfstrip[1][istrip-1][ilayer-1] = false;
            if (j_data.strips[istrip-1][ilayer-1]>comparator_threshold) {
                if (istrip>1 && istrip<NSTRIP) {
                    if ((j_data.strips[istrip-1][ilayer-1] > j_data.strips[istrip-2][ilayer-1]) && (j_data.strips[istrip-1][ilayer-1] > j_data.strips[istrip][ilayer-1])) {
                        if (j_data.strips[istrip-2][ilayer-1] > j_data.strips[istrip][ilayer-1]) {
                            softhalfstrip[0][istrip-1][ilayer-1] = true;
                        }
                        else {
                            softhalfstrip[1][istrip-1][ilayer-1] = true;
                        }
                    }
                }
                else if (istrip==1) {
                    if (j_data.strips[istrip-1][ilayer-1]>j_data.strips[istrip][ilayer]) {
                        softhalfstrip[1][istrip-1][ilayer-1] = true;
                    }
                }
                else if (istrip==NSTRIP) {
                    if (j_data.strips[istrip-1][ilayer-1]>j_data.strips[istrip-2][ilayer-1]) {
                        softhalfstrip[1][istrip-1][ilayer-1] = true;
                    }
                }
            }
        }
    }
}

void drawCtrigKeys(TVirtualPad* c) {
    TText* text = new TText;
    TBox* box = new TBox;
    TLine* line = new TLine;
    
    c->cd();
    text->SetTextFont(82);
    text->SetTextSize(.2);
    text->SetTextAlign(22);
    text->DrawText(.5, .9, "Layer 1");
    text->SetTextAngle(90);
    text->DrawText(.5, .6, "ADC Counts");
    text->SetTextAngle(0);
    text->SetTextSize(.19);
    text->DrawText(.5, .45, "Key");
    line->SetLineColor(1);
    line->DrawLine(.2, .44, .8, .44);
    text->SetTextSize(.14);
    text->SetTextAlign(11);
    text->SetTextFont(102);

    float btop=0.43;
    float bsize=0.08;
    //blue software halfstrip
    btop=0.43;
    box->SetFillStyle(1001);
    box->SetFillColor(4);
    box->DrawBox(.1, btop-bsize*1/3, .15, btop-bsize*0/3);
    box->SetFillStyle(0);
    box->SetFillColor(1);
    box->DrawBox(.1, btop-bsize, .15, btop);
    text->SetTextColor(4);
    text->DrawText(.2, .39, "Software");
    text->DrawText(.2, .37, "halfstrip");
    //pink hardware halfstrip
    btop=0.33;
    box->SetFillStyle(1001);
    box->SetFillColor(6);
    box->DrawBox(.1, btop-bsize*2/3, .15, btop-bsize*1/3);
    box->SetFillStyle(0);
    box->SetFillColor(1);
    box->DrawBox(.1, btop-bsize, .15, btop);
    text->SetTextColor(6);
    text->DrawText(.2, .29, "Hardware");
    text->DrawText(.2, .27, "halfstrip");
    //green	key halfstrip
    btop=0.23;
    box->SetFillStyle(1001);
    box->SetFillColor(3);
    box->DrawBox(.1, btop-bsize*3/3, .15, btop-bsize*2/3);
    box->SetFillStyle(0);
    box->SetFillColor(1);
    box->DrawBox(.1, btop-bsize, .15, btop);
    text->SetTextColor(1);
    text->DrawText(.2, .19, "Key");
    text->DrawText(.2, .17, "halfstrip");
    c->Update();
    
    delete text;
    delete box;
    delete line;
}

void drawCtrigAxis(TVirtualPad* c) {
    TLine* line = new TLine;
    TGaxis* axis = new TGaxis;
    TText* text = new TText;
    
    c->cd(2*NLAYER + 2);
    
    gPad->SetPad(gPad->GetXlowNDC(),0,gPad->GetXlowNDC()+gPad->GetWNDC(),1);
    gPad->SetFillStyle(4001);
    text->SetTextSize(0.03);
    text->SetTextFont(82);
    text->SetTextAlign(23);
    text->SetTextColor(1);
    text->DrawText(0.5, 0.05, "Strip Number");

    float histstop = 0.924;
    float histsbottom = 0.094;
    line->SetLineColor(1);
    line->SetLineWidth(1);
    line->SetLineStyle(1);
    line->DrawLine(striphist_x1, histsbottom, striphist_x1, histstop);
    line->DrawLine(striphist_x2, histsbottom, striphist_x2, histstop);

    // x-axis at the bottom
    axis->SetLabelSize(0.04);
    axis->SetLabelColor(1);
    axis->DrawAxis(striphist_x1, histsbottom, striphist_x2, histsbottom, 0, NSTRIP+2, 3206);

    line->SetLineColor(17);
    line->SetLineStyle(3);
    for(int i=1; i<NCFEB; ++i){
        double dx = (striphist_x2-striphist_x1)/(NSTRIP+2);
        double cfebedge = striphist_x1 + dx + dx*i*NCFEB_STRIP;
        line->DrawLine(cfebedge, histsbottom, cfebedge, histstop);
    }
    if(NCFEB==7){
        int i=4;
        double dx = (striphist_x2-striphist_x1)/(NSTRIP+2);
        double cfebedge = striphist_x1 + dx + dx*i*NCFEB_STRIP;
        line->SetLineColor(17);
        line->SetLineWidth(2);
        line->SetLineStyle(1);
        line->DrawLine(cfebedge, histsbottom-0.02, cfebedge, histstop+0.03);
        c->cd(1);
        text->SetTextSize(0.4);
        text->SetTextFont(82);
        text->SetTextAlign(23);
        text->SetTextColor(1);
        text->DrawText(0.35, 0.4, "ME1/1B");
        text->DrawText(0.80, 0.4, "ME1/1A");
    }
    c->Update();
    
    delete line;
    delete axis;
    delete text;
}

void scale_ctrig_y(j_common_type* j_data) {
    int i, layer_no;
    
    for (layer_no=1; layer_no<=6; layer_no++) {
        (*j_data).ymin[layer_no-1] = 0;
        (*j_data).ymax[layer_no-1] = 800;
        for (i=1; i<=NSTRIP; i++) {
            float value = (*j_data).strips[i-1][layer_no-1];
            if (value > (*j_data).ymax[layer_no-1]) {
                (*j_data).ymax[layer_no-1] = value;
            }
            // if (value < (*j_data).ymin[layer_no-1]) {
                // (*j_data).ymin[layer_no-1] = value;
            // }
        }
        float dScale = .3*((*j_data).ymax[layer_no-1] - (*j_data).ymin[layer_no-1]);
        (*j_data).ymax[layer_no-1] += dScale;
        // (*j_data).ymin[layer_no-1] -= dScale;
    }
}

/******** for Atrig ********/
void plotAtrigWires(TVirtualPad* c, data_type data, j_common_type j_data) {
    int i, j, ilayer, igroup;
    double dx, dy, x0, y0, x1, y1, x2, y2, ex, xk;
    char buffer[30];
    
    TBox* box = new TBox;
    TText* text = new TText;
    TLine* line = new TLine;
    
    c->cd();
    
    x0 = 0.20;
    y0 = 0.05;
    dx = 0.075;
    dy = 0.8/(NWIRE); //0.0075;
    
    x1 = x0;
    y1 = y0;
    x2 = x0 + dx;
    y2 = y0 + dy;
    box->SetFillStyle(1001);
    for (ilayer=1; ilayer<=6; ilayer++) {
        j = 2*ilayer - 1;
        for (igroup=1; igroup<=j_data.nch[j-1]; igroup++) {
            if (j_data.wires[igroup-1][ilayer-1]!=0) {
                ex = x1;
                box->SetFillColor(4);
                box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
            }
            if ((bool)j_data.wiregroup[igroup-1][ilayer-1]) {
                ex = x1 + .25*dx;
                box->SetFillColor(3);
                box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
            }
            if ((bool)j_data.alct[0][igroup-1][ilayer-1]) {
                ex = x1 + .5*dx;
                box->SetFillColor(6);
                box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
            }
            if ((bool)j_data.alct[1][igroup-1][ilayer-1]) {
                ex = x1 + .5*dx;
                box->SetFillStyle(3002);
                box->SetFillColor(6);
                box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
                box->SetFillStyle(1001);
            }
            if ((bool)j_data.tmba[igroup-1][ilayer-1]) {
                ex = x1 + .75*dx;
                box->SetFillColor(2);
                box->DrawBox(ex, y1, ex+.25*dx, y1+dy);
            }
            y1 += dy;
        }
        x1 = x1 + dx + .01;
        y1 = y0;
    }

    box->SetFillStyle(0);
    box->SetFillColor(1);

    x1 = x0;
    y1 = y0;
    x2 = x0 + dx;
    y2 = y0 + dy;

    text->SetTextFont(42);

    text->SetTextAlign(32);
    text->SetTextSize(.03);
    sprintf(buffer, "Wire Group 1");
    text->DrawText(x0-0.01, y0, buffer);

    sprintf(buffer, "Wire Group %d", NWIRE);
    text->DrawText(x0-0.01, y0 + (NWIRE-1)*dy, buffer);

    line->DrawLine(x1, y1, x1+NLAYER*(dx+.01)-.01, y1);

    text->SetTextAlign(11);
    for (i=1; i<=NLAYER; i++) {
        for (j=1; j<=NWIRE; j++) {
            box->DrawBox(x1, y1, x2, y2);
            y1 += dy;
            y2 += dy;
            
            if (i==1&&(j==NWIRE||j%10==0)) {
                line->DrawLine(x1, y1, x1+NLAYER*(dx+.01)-.01, y1);
            }
        }

        if (i==1 || i==NLAYER) {
            text->SetTextSize(.03);
            sprintf(buffer, "Layer %d", i);
            text->DrawText(x1, y1+.008, buffer);
        }
        
        x1 = x1 + dx + .01;
        x2 = x1 + dx;
        y1 = y0;
        y2 = y0 + dy;
    }

    text->SetTextSize(0.04);
    text->DrawText(0.03, 0.55, "ANODE");
    text->DrawText(0.03, 0.5, "TRIGGER");
    text->DrawText(0.03, 0.45, "DISPLAY");

    //KEY
    float keybottom = 0.1;
    float keyheight = 0.6;

    text->SetTextAlign(22);
    xk = x0+(NLAYER+1)*(dx+0.01);
    dy*=2;

    text->SetTextSize(0.04);
    text->DrawText(xk+0.5*dx, keybottom + keyheight, "KEY");
    line->DrawLine(xk-0.5*dx, keybottom + keyheight*0.95,xk+1.5*dx, keybottom + keyheight*0.95);

    text->SetTextSize(0.03);
    box->SetFillStyle(1001);
    box->SetFillColor(4);
    box->DrawBox(xk, keybottom + keyheight*0.85, xk+0.25*dx, keybottom + keyheight*0.85+dy);
    box->SetFillStyle(0);
    box->SetFillColor(1);
    box->DrawBox(xk, keybottom + keyheight*0.85, xk+dx, keybottom + keyheight*0.85+dy);
    text->SetTextColor(4);
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.8, "The wire had");
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.75, "a TDC hit");

    box->SetFillStyle(1001);
    box->SetFillColor(3);
    box->DrawBox(xk+0.25*dx, keybottom + keyheight*0.65, xk+0.5*dx, keybottom + keyheight*0.65+dy);
    box->SetFillStyle(0);
    box->SetFillColor(1);
    box->DrawBox(xk, keybottom + keyheight*0.65, xk+dx, keybottom + keyheight*0.65+dy);
    text->SetTextColor(3);
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.6, "The wire appears");
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.55, "in the ALCT");
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.5, "raw hits data");

    box->SetFillStyle(1001);
    box->SetFillColor(6);
    box->DrawBox(xk+0.5*dx, keybottom + keyheight*0.4, xk+0.75*dx, keybottom + keyheight*0.4+dy);
    box->SetFillStyle(3002);
    box->DrawBox(xk+0.5*dx, keybottom + keyheight*0.35, xk+0.75*dx, keybottom + keyheight*0.35+dy);
    box->SetFillStyle(0);
    box->SetFillColor(1);
    box->DrawBox(xk, keybottom + keyheight*0.4, xk+dx, keybottom + keyheight*0.4+dy);
    box->DrawBox(xk, keybottom + keyheight*0.35, xk+dx, keybottom + keyheight*0.35+dy);
    text->SetTextColor(6);
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.3, "The best and 2nd");
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.25, "best LCT pattern");
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.2, "matched in this event");

    box->SetFillStyle(1001);
    box->SetFillColor(2);
    box->DrawBox(xk+0.75*dx, keybottom + keyheight*0.1, xk+dx, keybottom + keyheight*0.1+dy);
    box->SetFillStyle(0);
    box->SetFillColor(1);
    box->DrawBox(xk, keybottom + keyheight*0.1, xk+dx, keybottom + keyheight*0.1+dy);
    text->SetTextColor(2);
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.05, "The wire was part of");
    text->DrawText(xk+0.5*dx, keybottom + keyheight*0.0, "the TMB choice");

    text->SetTextColor(1);
    text->SetTextAlign(32);
    sprintf(buffer, "ALCT0 quality %d", j_data.alct_q[0]);
    text->DrawText(0.95, 0.85, buffer);
    sprintf(buffer, "ALCT1 quality %d", j_data.alct_q[1]);
    text->DrawText(0.95, 0.8, buffer);

    c->Update();
    
    delete box;
    delete text;
    delete line;
}

void atrigWireGeomCc(j_common_type* j_data) {
    int i;
    float dy_even, dy_odd, ymid_index;
    dy_odd = .0141;
    dy_even = .0025;
    ymid_index = 0.0 + .5*dy_odd;
    
    (*j_data).x0 = 1;
    (*j_data).dx = .10*(48.0/NWIRE);
    for (i=1; i<=11; i+=2) {
        (*j_data).nch[i-1] = NWIRE;
    }
    for (i=2; i<=10; i+=2) {
        (*j_data).nch[i-1] = -NWIRE;
    }
    for (i=1; i<=11; i++) {
        (*j_data).ymid[i-1] = ymid_index;
        ymid_index = ymid_index +.5*(dy_even+dy_odd);
    }
    return;
}


/************************** Other Supporting Functions **************************/

void scale_y(j_common_type* j_data, bool peak) {
    for (int ilayer = 0; ilayer < 6; ilayer++) {
        (*j_data).ymin[ilayer]=0.0;
        (*j_data).ymax[ilayer]=50.0;
        for (int j = 0; j < NSTRIP; j++) {
            
            // decide which data to pick (max or peak)
            float value;
            if (peak)   value = (*j_data).strips_peaks[j][ilayer];
            else        value = (*j_data).strips[j][ilayer];
            
            if (value > (*j_data).ymax[ilayer]) {
                (*j_data).ymax[ilayer] = value;
            }
            // if (value < (*j_data).ymin[ilayer]) {
                // (*j_data).ymin[ilayer] = value;
            // }
        }
        float dScale = .3*((*j_data).ymax[ilayer] - (*j_data).ymin[ilayer]);
        (*j_data).ymax[ilayer] += dScale;
        // (*j_data).ymin[ilayer] -= dScale;
    }
}

void getCanvasBuffer(TVirtualPad* c, char** imgBuf, int* size) {
    // create an image from canvas
    gSystem->ProcessEvents();
    TImage* img = TImage::Create();
    img->FromPad(c);
    
    // get image buffer from png
    img->GetImageBuffer(imgBuf, size, TImage::kPng);
    
    delete img;
}
