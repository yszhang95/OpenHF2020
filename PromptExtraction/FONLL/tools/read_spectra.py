#!/usr/bin/env python3
if __name__ == '__main__':
    ymins = [-3.46, -2.46, -1.46, -0.46, 0.54, 1.54]
    ymaxs = [-2.46, -1.46, -0.46, 0.54, 1.54, 2.54]
    ptmins = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
              15, 16, 18, 20, 22, 24, 26, 28]
    ptmaxs = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
              15, 16, 18, 20, 22, 24, 26, 28, 30]

    for ymin, ymax in zip(ymins, ymaxs):
        foutstr = 'output_pp_8p16TeV/pTspectra_y{ymin}to{ymax}'.format(ymin=ymin, ymax=ymax)
        foutstr2 = 'output_pp_8p16TeV/pTspectra_int_y{ymin}to{ymax}'.format(ymin=ymin, ymax=ymax)
        foutstr = foutstr.replace(".", 'p')
        foutstr2 = foutstr2.replace(".", 'p')
        foutstr = foutstr + ".txt"
        foutstr2 = foutstr2 + ".txt"
        fout = open(foutstr, 'w')
        fout2 = open(foutstr2, 'w')
        for ptmin, ptmax in zip(ptmins, ptmaxs):
            outstr = "output_pp_8p16TeV/pT{ptmin}to{ptmax}_y{ymin}to{ymax}".format(
                      ptmin=ptmin, ptmax=ptmax, ymin=ymin, ymax=ymax)
            outstr = outstr.replace(".", "p")
            outstr = outstr + ".txt"
            with open(outstr) as f:
                lines = []
                for l in f.readlines():
                    if not l[0] == '#':
                        lines.append(l.strip())
                # print(lines)
                nums_min = [ float(l) for l in lines[0].split(' ')]
                # first pt, second central, third min, fourth max
                pt = (ptmin + ptmax) / 2
                pt_err = (ptmax - ptmin) / 2
                xsec = nums_min[1]/(ptmax - ptmin)
                xsec_lo = nums_min[2]/(ptmax - ptmin)
                xsec_hi = nums_min[3]/(ptmax - ptmin)
                outnums = "{pt} {xsec}  {pt_err} {pt_err} {lo} {hi}\n".format(pt=pt, pt_err=pt_err, xsec=xsec, lo=xsec-xsec_lo, hi=xsec_hi-xsec)
                fout.write(outnums)

                xsec = nums_min[1]
                xsec_lo = nums_min[2]
                xsec_hi = nums_min[3]
                outnums = "{pt} {xsec}  {pt_err} {pt_err} {lo} {hi}\n".format(pt=pt, pt_err=pt_err, xsec=xsec, lo=xsec-xsec_lo, hi=xsec_hi-xsec)
                fout2.write(outnums)
